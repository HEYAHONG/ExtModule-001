/***************************************************************
 * Name:      hstacklesscoroutine.c
 * Purpose:   实现hstacklesscoroutine接口
 * Author:    HYH (hyhsystem.cn)
 * Created:   2024-08-24
 * Copyright: HYH (hyhsystem.cn)
 * License:   MIT
 **************************************************************/
#include "hstacklesscoroutine.h"
#include "stdlib.h"

bool hstacklesscoroutine_is_finished(hstacklesscoroutine_control_block_t *ccb)
{
    if(ccb!=NULL)
    {
        return (ccb->flags&(0x1ULL<<1))!=0;
    }
    return true;
}

void hstacklesscoroutine_coroutine_restart(hstacklesscoroutine_control_block_t *ccb)
{
    if(ccb!=NULL)
    {
        if(hstacklesscoroutine_is_finished(ccb))
        {
            ccb->flags=0;
            ccb->corevalue=0;
        }
    }
}

void hstacklesscoroutine_coroutine_force_restart(hstacklesscoroutine_control_block_t *ccb)
{
    if(ccb!=NULL)
    {
        ccb->flags=0;
        ccb->corevalue=0;
    }
}

bool hstacklesscoroutine_is_suspend(hstacklesscoroutine_control_block_t *ccb)
{
    if(ccb!=NULL)
    {
        return (ccb->flags &(0x1ULL<<0))!=0;
    }
    return false;
}
void hstacklesscoroutine_coroutine_suspend(hstacklesscoroutine_control_block_t *ccb)
{
    if(ccb!=NULL)
    {
        ccb->flags|=(0x1ULL << 0);
    }
}
void hstacklesscoroutine_coroutine_resume(hstacklesscoroutine_control_block_t *ccb)
{
    if(ccb!=NULL)
    {
        ccb->flags&=~(0x1ULL << 0);
    }
}


bool hstacklesscoroutine_is_await(hstacklesscoroutine_control_block_t *ccb)
{
    if(ccb!=NULL)
    {
        return ccb->awaiter.wait_for_ready!=NULL;
    }
    return false;
}