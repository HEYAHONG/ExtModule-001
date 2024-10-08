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
    if(addr < 16)
    {
        //GPIOA
        return GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0<<(addr%16));
    }
    if(addr < 16*2)
    {
        //GPIOB
        return GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0<<(addr%16));
    }
    if(addr < 16*3)
    {
        //GPIOC
        return GPIO_PIN_RESET!=HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0<<(addr%16));
    }
    return addr%2!=0;
}
static bool    read_discrete_input(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    return addr%2==0;
}
static modbus_data_register_t  holding_registers[6]= {0};
static modbus_data_register_t  read_holding_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    if(addr < sizeof(holding_registers)/sizeof(holding_registers[0]))
    {
        return holding_registers[addr];
    }
    return 0xDEAD;
}
extern "C" uint16_t ADCConvert(uint8_t channel);
static modbus_data_register_t  read_input_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr)
{
    if(addr == 0)
    {
        return HAL_GetUIDw0();
    }
    if(addr == 1)
    {
        return HAL_GetUIDw0()>>16;
    }
    if(addr == 2)
    {
        return HAL_GetUIDw1();
    }
    if(addr == 3)
    {
        return HAL_GetUIDw1()>>16;
    }
    if(addr == 4)
    {
        return HAL_GetUIDw2();
    }
    if(addr == 5)
    {
        return HAL_GetUIDw2()>>16;
    }
    if(addr > 5 && addr < 17)
    {
        //[6 15]��ʾ[ADC_Channel_0 ADC_Channel_10]
        return ADCConvert(addr-6);
    }
    return 0xBEEF;
}
static void  write_coil(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr,bool value)
{
    if(addr < 16)
    {
        //GPIOA
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0<<(addr%16),value?GPIO_PIN_SET:GPIO_PIN_RESET);
        return;
    }
    if(addr < 16*2)
    {
        //GPIOB
        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0<<(addr%16),value?GPIO_PIN_SET:GPIO_PIN_RESET);
        return;
    }
    if(addr < 16*3)
    {
        //GPIOC
        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0<<(addr%16),value?GPIO_PIN_SET:GPIO_PIN_RESET);
        return;
    }
}
static void  write_holding_register(modbus_rtu_slave_tiny_context_t* ctx,modbus_data_address_t addr,modbus_data_register_t value)
{
    if(addr < sizeof(holding_registers)/sizeof(holding_registers[0]))
    {
        holding_registers[addr]=value;
    }
}

static bool check_anycast_condition(modbus_rtu_slave_tiny_context_t* ctx)
{
    for(size_t i=0; i<6; i++)
    {
        if(holding_registers[i]!=read_input_register(ctx,i))
        {
            return false;
        }
    }
    return true;
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
ctx=modbus_rtu_slave_tiny_context_default();
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
ctx.check_anycast_condition=check_anycast_condition;
{
    //��ʼ�����ּĴ���
    for(size_t i=0; i<sizeof(holding_registers)/sizeof(holding_registers[0]); i++)
    {
        holding_registers[i]=read_input_register(&ctx,i);
    }
}
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

