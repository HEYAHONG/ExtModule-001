#include "main.h"
#include "hbox.h"

static void hsoftplc_callback(hsoftplc_callback_type_t cb_type)
{
    switch(cb_type)
    {
    case HSOFTPLC_CALLBACK_TYPE_CONFIG_INIT_BEGIN:
    {

    }
    break;
    case HSOFTPLC_CALLBACK_TYPE_CONFIG_INIT_END:
    {

    }
    break;
    case HSOFTPLC_CALLBACK_TYPE_CONFIG_RUN_BEGIN:
    {
        hsoftplc_get_located_all_variables([](const char *name,void *var,void *usr)
        {
            hsoftplc_variable_symbol_t variable_symbol;
            if(hsoftplc_parse_variable_symbol(&variable_symbol,name)!=NULL)
            {
                if(variable_symbol.variable_location == 'I' && variable_symbol.variable_size=='X')
                {
                    if(variable_symbol.variable_address[0]!=NULL && variable_symbol.variable_address[1]!=NULL)
                    {
                        int addr0=atoi(variable_symbol.variable_address[0]);
                        int addr1=atoi(variable_symbol.variable_address[1]);
                        if(addr0==0)
                        {
                            hsoftplc_database_value_t value=0;
                            switch(addr1)
                            {
                            case 0:
                            {
                                /*
                                 * %IX0.0 PB2
                                 */
                                value=((GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2))?0x01:0x00);
                            }
                            break;
                            case 1:
                            {
                                /*
                                 * %IX0.1 PA0
                                 */
                                value=((GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))?0x01:0x00);
                            }
                            break;
                            case 2:
                            {
                                /*
                                 * %IX0.2 PA1
                                 */
                                value=((GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1))?0x01:0x00);
                            }
                            break;
                            case 3:
                            {
                                /*
                                 * %IX0.3 PA5
                                 */
                                value=((GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5))?0x01:0x00);
                            }
                            break;
                            default:
                            {

                            }
                            break;
                            }
                            hsoftplc_database_set_value(hsoftplc_database_key_get_from_variable_name(name),var,value);
                        }
                    }
                }
            }

        },NULL);

    }
    break;
    case HSOFTPLC_CALLBACK_TYPE_CONFIG_RUN_END:
    {
        hsoftplc_get_located_all_variables([](const char *name,void *var,void *usr)
        {
            hsoftplc_variable_symbol_t variable_symbol;
            if(hsoftplc_parse_variable_symbol(&variable_symbol,name)!=NULL)
            {
                if(variable_symbol.variable_location == 'Q' && variable_symbol.variable_size=='X')
                {
                    if(variable_symbol.variable_address[0]!=NULL && variable_symbol.variable_address[1]!=NULL)
                    {
                        int addr0=atoi(variable_symbol.variable_address[0]);
                        int addr1=atoi(variable_symbol.variable_address[1]);
                        if(addr0==0)
                        {
                            if(addr1==0)
                            {
                                /*
                                 * %QX0.0
                                 */
                                hsoftplc_database_value_t value=0;
                                if(hsoftplc_database_get_value(hsoftplc_database_key_get_from_variable_name(name),var,&value))
                                {
                                    /*
                                     * PB5
                                     */
                                    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,value!=0?GPIO_PIN_SET:GPIO_PIN_RESET);
                                }
                            }
                        }
                    }
                }
            }
        },NULL);
    }
    break;
    }
}


enum
{
    SOFTPLC_LOOP_START=1,
    SOFTPLC_LOOP_EXIT,
};

HSTACKLESSCOROUTINE_BLOCK_START(softplc)
/*
 * 设置软件PLC回调
 */
hsoftplc_set_callback(hsoftplc_callback);
/*
 * 初始化软件PLC
 */
hsoftplc_init();
hstacklesscoroutine_yield_with_label(SOFTPLC_LOOP_START);
hsoftplc_loop();
hstacklesscoroutine_goto_label(SOFTPLC_LOOP_START);
hstacklesscoroutine_yield_with_label(SOFTPLC_LOOP_EXIT);
HSTACKLESSCOROUTINE_BLOCK_END(softplc)

