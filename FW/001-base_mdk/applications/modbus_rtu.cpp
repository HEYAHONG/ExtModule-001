#include "main.h"
#include "hbox.h"

/*
 * modbus�ص�����,ʵ��ʹ��ʱ��Ҫ������ʵ�ʵ�ҵ���߼����˴�ֻ��һЩ�ղ���/������
 */
static void    reply(modbus_rtu_slave_tiny_context_t* ctx,const uint8_t *adu,size_t adu_length)
{
    UART_HandleTypeDef *Handle=(UART_HandleTypeDef *)ctx->usr;
    if(Handle!=NULL)
    {
        HAL_UART_Transmit(Handle,(uint8_t *)adu,adu_length,30000);
    }
}
static bool    read_coil(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    return addr%2!=0;
}
static bool    read_discrete_input(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    return addr%2==0;
}
static modbus_data_register_t  read_holding_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    return addr+1;
}
static modbus_data_register_t  read_input_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    return addr+2;
}
static void  write_coil(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr,bool value)
{

}
static void  write_holding_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr,modbus_data_register_t value)
{

}

//modbus������
static modbus_rtu_slave_tiny_context ctx= {0};
enum
{
    MODBUS_RTU_LOOP_START=1,
    MODBUS_RTU_LOOP_EXIT,
};
static uint8_t *modbus_rx_buffer=NULL;
static __IO size_t modbus_rx_index=0;
static hdefaults_tick_t last_tick=0;
HSTACKLESSCOROUTINE_BLOCK_START(modbus_rtu)
//��ʼ��modbus����
ctx.addr=MODBUS_NODE_ADDRESS_DEFAULT;
ctx.buffer=(uint8_t *)hdefaults_malloc(MODBUS_RTU_MAX_ADU_LENGTH,NULL);//���뷢�ͻ���
modbus_rx_buffer=(uint8_t *)hdefaults_malloc(MODBUS_RTU_MAX_ADU_LENGTH,NULL);
if(modbus_rx_buffer==NULL || ctx.buffer == NULL)
{
    //Э���˳�
    hstacklesscoroutine_goto_label(MODBUS_RTU_LOOP_EXIT);
}
ctx.usr=HSTACKLESSCOROUTINE_GET_CURRENT_EVENT()->eventparam;//���ݴ��ھ��
ctx.reply=reply;
ctx.read_coil=read_coil;
ctx.read_discrete_input=read_discrete_input;
ctx.read_holding_register=read_holding_register;
ctx.read_input_register=read_input_register;
ctx.write_coil=write_coil;
ctx.write_holding_register=write_holding_register;
hstacklesscoroutine_yield_with_label(MODBUS_RTU_LOOP_START)
if(HAL_UART_Receive((UART_HandleTypeDef *)HSTACKLESSCOROUTINE_GET_CURRENT_EVENT()->eventparam,&modbus_rx_buffer[modbus_rx_index],1,0)==HAL_OK)
{
    //���յ�����
    last_tick=hdefaults_tick_get();
    if(modbus_rx_index<MODBUS_RTU_MAX_ADU_LENGTH)
    {
        modbus_rx_index++;
    }
    else
    {
        modbus_rtu_slave_tiny_parse_input(&ctx,modbus_rx_buffer,modbus_rx_index);
        modbus_rx_index=0;
    }
}
else
{
    UART_HandleTypeDef * Handle=(UART_HandleTypeDef *)HSTACKLESSCOROUTINE_GET_CURRENT_EVENT()->eventparam;
    hdefaults_tick_t timeout=1000*7/2/(Handle->Init.BaudRate/10);//3.5�ַ�ʱ��
    if(timeout < 1)
    {
        timeout=1;
    }
    if(hdefaults_tick_get()-last_tick > timeout)
    {
        if(modbus_rx_index!=0)
        {
            modbus_rtu_slave_tiny_parse_input(&ctx,modbus_rx_buffer,modbus_rx_index);
            modbus_rx_index=0;
        }
    }
}
hstacklesscoroutine_goto_label(MODBUS_RTU_LOOP_START);
hstacklesscoroutine_yield_with_label(MODBUS_RTU_LOOP_EXIT);
HSTACKLESSCOROUTINE_BLOCK_END(modbus_rtu)

