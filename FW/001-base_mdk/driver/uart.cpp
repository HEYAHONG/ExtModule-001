#include "main.h"
#include "hbox.h"

UART_HandleTypeDef UartHandle= {0};
static void uart_init()
{
    {
        //IO及复用
        GPIO_InitTypeDef  GPIO_InitStruct = {0};

        /* Clock Enable */
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        /* GPIO initialization
        PA03：TX,
        PA04：RX
        */
        GPIO_InitStruct.Pin       = GPIO_PIN_3;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    }
    {
        //设置串口参数
        UartHandle.Instance          = USART1;
        UartHandle.Init.BaudRate     = 115200;
        UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
        UartHandle.Init.StopBits     = UART_STOPBITS_1;
        UartHandle.Init.Parity       = UART_PARITY_NONE;
        UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
        UartHandle.Init.Mode         = UART_MODE_TX_RX;
        UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
        if (HAL_UART_Init(&UartHandle) != HAL_OK)
        {
            //串口初始化失败
            while(true);
        }
    }
}
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&UartHandle);
}
enum
{
    UART_LOOP_START=1,
};
//对于串口协议的协程，事件的参数为串口句柄
hstacklesscoroutine_event_t uart_event= {(intptr_t)USART1,&UartHandle};
HSTACKLESSCOROUTINE_DECLARE_COROUTINE(modbus_rtu);
HSTACKLESSCOROUTINE_BLOCK_START(uart)
uart_init();
hstacklesscoroutine_yield_with_label(UART_LOOP_START);
//此处运行串口协议,默认运行modbus rtu协议,如需其它协议请注释掉下一行。
HSTACKLESSCOROUTINE_ENTRY_WITH_CCB_AND_EVENT(modbus_rtu,HSTACKLESSCOROUTINE_GET_GLOBAL_CCB(modbus_rtu),&uart_event);

hstacklesscoroutine_goto_label(UART_LOOP_START);
HSTACKLESSCOROUTINE_BLOCK_END(uart)
