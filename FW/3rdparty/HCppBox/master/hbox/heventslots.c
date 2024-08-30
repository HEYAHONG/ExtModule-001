/***************************************************************
 * Name:      heventslots.c
 * Purpose:   实现heventslots接口
 * Author:    HYH (hyhsystem.cn)
 * Created:   2023-07-14
 * Copyright: HYH (hyhsystem.cn)
 * License:   MIT
 **************************************************************/

#include "heventslots.h"
#include "hdefaults.h"

typedef struct heventslots_slot
{
    //用户参数
    void *usr;
    //槽函数,第一个参数为signal指针(由发射信号时指定),第二个参数为用户参数
    void (*slot)(void *,void *);
    //槽释放,第一个参数为用户参数
    void (*onfree)(void *);
    //下一个槽指针
    struct heventslots_slot *next;
    //槽id
    uint32_t id;
} heventslots_slot_t;

struct heventslots
{
    //用户参数
    void *usr;
    //获取内存,第一个参数为待获取的内存大小,第二个参数为用户参数。
    void *(*mem_alloc)(size_t,void *);
    //释放内存,第一个参数为待释放的内存指针,第二个参数为用户参数。
    void (*mem_free)(void *,void *);
    //锁,参数为用户参数。
    void (*mutex_lock)(void *);
    //解锁,参数为用户参数。
    void (*mutex_unlock)(void *);
    //槽单向链表指针
    heventslots_slot_t *slot_start;
    //下一个槽id
    uint32_t slot_next_id;
};

heventslots_t * heventslots_new_with_memmang_and_lock(void *usr,void *(*mem_alloc)(size_t,void *),void (*mem_free)(void *,void *),void (*mutex_lock)(void *),void (*mutex_unlock)(void *))
{
    struct heventslots *slots=NULL;
    if(mem_alloc!=NULL)
    {
        slots=(struct heventslots *)mem_alloc(sizeof(struct heventslots),usr);
    }
    else
    {
        slots=(struct heventslots *)malloc(sizeof(struct heventslots));
    }

    if(slots==NULL)
    {
        return NULL;
    }

    memset(slots,0,sizeof(struct heventslots));

    slots->usr=usr;
    slots->mem_alloc=mem_alloc;
    slots->mem_free=mem_free;
    slots->mutex_lock=mutex_lock;
    slots->mutex_unlock=mutex_unlock;
    slots->slot_start=NULL;
    slots->slot_next_id=1;

    return slots;
}
heventslots_t * heventslots_new_with_memmang(void *usr,void *(*mem_alloc)(size_t,void *),void (*mem_free)(void *,void *))
{
    return heventslots_new_with_memmang_and_lock(usr,mem_alloc,mem_free,hdefaults_mutex_lock,hdefaults_mutex_unlock);
}
heventslots_t * heventslots_new_with_lock(void *usr,void (*mutex_lock)(void *),void (*mutex_unlock)(void *))
{
    return heventslots_new_with_memmang_and_lock(usr,hdefaults_malloc,hdefaults_free,mutex_lock,mutex_unlock);
}
heventslots_t * heventslots_new(void *usr)
{
    return heventslots_new_with_memmang_and_lock(usr,hdefaults_malloc,hdefaults_free,hdefaults_mutex_lock,hdefaults_mutex_unlock);
}

void * heventslots_get_usr_ptr(heventslots_t *slots)
{
    if(slots==NULL)
    {
        return NULL;
    }
    return slots->usr;
}

void heventslots_free(heventslots_t *slots)
{
    if(slots==NULL)
    {
        return;
    }

    heventslots_unregister_all_slot(slots);

    if(slots->mem_free!=NULL)
    {
        slots->mem_free(slots,slots->usr);
    }
    else
    {
        free(slots);
    }
}


void heventslots_emit_signal(heventslots_t *slots,void *signal)
{
    if(slots==NULL)
    {
        return;
    }

    //加锁
    if(slots->mutex_lock!=NULL)
    {
        slots->mutex_lock(slots->usr);
    }

    heventslots_slot_t *slot=slots->slot_start;

    while(slot!=NULL)
    {
        if(slot->slot!=NULL)
        {
            slot->slot(signal,slot->usr);
        }
        slot=slot->next;
    }

    //解锁
    if(slots->mutex_unlock!=NULL)
    {
        slots->mutex_unlock(slots->usr);
    }
}

uint32_t heventslots_register_slot(heventslots_t *slots,void *slot_usr,void (*onslot)(void *,void *),void (*onfree)(void *))
{
    if(slots==NULL)
    {
        return 0;
    }

    heventslots_slot_t *slot=NULL;

    if(slots->mem_alloc!=NULL)
    {
        slot=(heventslots_slot_t *)slots->mem_alloc(sizeof(heventslots_slot_t),slots->usr);
    }
    else
    {
        slot=(heventslots_slot_t *)malloc(sizeof(heventslots_slot_t));
    }

    if(slot==NULL)
    {
        return 0;
    }

    slot->usr=slot_usr;
    slot->slot=onslot;
    slot->onfree=onfree;
    slot->id=slots->slot_next_id++;
    slot->next=NULL;

    if(slot->id==0)
    {
        slot->id=slots->slot_next_id++;
    }

    //加锁
    if(slots->mutex_lock!=NULL)
    {
        slots->mutex_lock(slots->usr);
    }

    heventslots_slot_t *slots_slot=slots->slot_start;

    while(slots_slot!=NULL)
    {
        if(slots_slot->next==NULL)
        {
            slots_slot->next=slot;
            break;
        }
        slots_slot=slots_slot->next;
    }

    if(slots_slot==NULL)
    {
        slots->slot_start=slot;
    }

    //解锁
    if(slots->mutex_unlock!=NULL)
    {
        slots->mutex_unlock(slots->usr);
    }

    return slot->id;
}

void heventslots_unregister_slot(heventslots_t *slots,uint32_t id)
{
    if(slots==NULL)
    {
        return;
    }

    //加锁
    if(slots->mutex_lock!=NULL)
    {
        slots->mutex_lock(slots->usr);
    }

    heventslots_slot_t *slots_slot=slots->slot_start;

    if(slots_slot!=NULL && slots_slot->id==id)
    {
        heventslots_slot_t *slot=slots_slot->next;
        if(slots_slot->onfree!=NULL)
        {
            slots_slot->onfree(slots_slot->usr);
        }
        if(slots->mem_free!=NULL)
        {
            slots->mem_free(slots_slot,slots->usr);
        }
        else
        {
            free(slots_slot);
        }
        slots->slot_start=slot;
        slots_slot=NULL;
    }

    while(slots_slot!=NULL)
    {
        if(slots_slot->next==NULL)
        {
            break;
        }

        if(slots_slot->next->id==id)
        {
            heventslots_slot_t *slot=slots_slot->next;
            slots_slot->next=slots_slot->next->next;
            if(slot->onfree!=NULL)
            {
                slot->onfree(slot->usr);
            }
            if(slots->mem_free!=NULL)
            {
                slots->mem_free(slot,slots->usr);
            }
            else
            {
                free(slot);
            }
            break;
        }

        slots_slot=slots_slot->next;
    }


    //解锁
    if(slots->mutex_unlock!=NULL)
    {
        slots->mutex_unlock(slots->usr);
    }

}

void heventslots_unregister_all_slot(heventslots_t *slots)
{
    if(slots==NULL)
    {
        return;
    }

    //加锁
    if(slots->mutex_lock!=NULL)
    {
        slots->mutex_lock(slots->usr);
    }

    heventslots_slot_t *slot=slots->slot_start;
    slots->slot_start=NULL;
    while(slot!=NULL)
    {
        heventslots_slot_t *next_slot=slot->next;
        if(slot->onfree!=NULL)
        {
            slot->onfree(slot->usr);
        }
        if(slots->mem_free!=NULL)
        {
            slots->mem_free(slot,slots->usr);
        }
        else
        {
            free(slot);
        }
        slot=next_slot;
    }

    slots->slot_next_id=1;

    //解锁
    if(slots->mutex_unlock!=NULL)
    {
        slots->mutex_unlock(slots->usr);
    }

}
