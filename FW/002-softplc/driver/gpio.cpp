#include "main.h"
#include "hbox.h"

static void gpio_init()
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    {
        GPIO_InitTypeDef  GPIO_InitStruct = {0};
        //初始化输入
        GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull      = GPIO_NOPULL ;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        {
            //I0 PB2
            GPIO_InitStruct.Pin=GPIO_PIN_2;
            HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
        }
        {
            //I1 PA0
            GPIO_InitStruct.Pin=GPIO_PIN_0;
            HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
        }
        {
            //I2 PA1
            GPIO_InitStruct.Pin=GPIO_PIN_1;
            HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
        }
        {
            //I3 PA5
            GPIO_InitStruct.Pin=GPIO_PIN_5;
            HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
        }
        //输出初始化
        GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
        {
            //O0 PB5
            GPIO_InitStruct.Pin=GPIO_PIN_5;
            HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
        }
    }
}
HSTACKLESSCOROUTINE_BLOCK_START(gpio)
gpio_init();
HSTACKLESSCOROUTINE_BLOCK_END(gpio)
