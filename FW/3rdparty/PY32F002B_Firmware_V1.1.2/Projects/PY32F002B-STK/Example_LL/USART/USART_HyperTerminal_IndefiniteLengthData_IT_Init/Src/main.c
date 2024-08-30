/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "py32f002bxx_ll_Start_Kit.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t RxBuf[RX_MAX_LEN] = {0}; /* Receive buffer */
uint32_t RxLen = 0;              /* Single frame data, actual received data length */
__IO uint32_t CheckFlag = 0;     /* After the hardware detects an idle frame, the
                                    "End of single frame data reception" check flag */
uint32_t CheckLen = 0;           /* Used to determine whether new data has arrived 
                                    within the timeout period */
uint32_t timeout = RX_TIMEOUT;   /* Used for timeout judgment */

uint8_t *TxBuff = NULL;
__IO uint16_t TxSize = 0;
__IO uint16_t TxCount = 0;

/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigUsart(USART_TypeDef *USARTx);
static void APP_UartRxTimeOut(void);
static void APP_UsartTransmit(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);

/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void)
{
  /* Configure Systemclock */
  APP_SystemClockConfig(); 

  /* Configure USART1 */
  APP_ConfigUsart(USART1);
  
  /* Enable idle frame interrupt */
  LL_USART_EnableIT_IDLE(USART1);
  
  /* Enable RX Not Empty Interrupt */
  LL_USART_EnableIT_RXNE(USART1);

  while (1)
  {
    /* Uart receive timeout check */
    APP_UartRxTimeOut();
  }
}

/**
  * @brief  Configure Systemclock
  * @param  None
  * @retval None
  */
static void APP_SystemClockConfig(void)
{
  /* Enable HSI */
  LL_RCC_HSI_Enable();
  while(LL_RCC_HSI_IsReady() != 1)
  {
  }

  /* Set AHB divider:HCLK = SYSCLK*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  /* HSISYS used as SYSCLK source */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
  {
  }

  /* Set APB1 divider */
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_Init1msTick(24000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(24000000);
}

/**
  * @brief  USART2 configuration function
  * @param  None
  * @retval None
  */
static void APP_ConfigUsart(USART_TypeDef *USARTx)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  LL_USART_InitTypeDef USART_InitStruct = {0};
  /*Enable GPIOB clock*/
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
  /* Enable USART1 clock */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

  /* Select pin 4 */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
  /* Select alternate mode */
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  /* Set output speed */
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  /* Set output type to push pull */
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  /* Enable pull up */
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  /* Set alternate function to USART1 function  */
  GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
  /* Initialize GPIOB */
  LL_GPIO_Init(GPIOB,&GPIO_InitStruct);

  /* Select pin 5 */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  /* Set alternate function to USART1 function */
  GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
  /* Initialize GPIOB */
  LL_GPIO_Init(GPIOB,&GPIO_InitStruct);

  /* Set USART1 interrupt priority  */
  NVIC_SetPriority(USART1_IRQn,0);
  /* Enable USART1 interrupt request */
  NVIC_EnableIRQ(USART1_IRQn);
  
  /* Set USART feature */
  /* Set baud rate */
  USART_InitStruct.BaudRate = 9600;
  /* set word length to 8 bits: Start bit, 8 data bits, n stop bits */
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  /* 1 stop bit */
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
 /* Parity control disabled  */
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  /* Initialize USART */
  LL_USART_Init(USARTx, &USART_InitStruct);
  
  /* Set mode as full-duplex asynchronous mode */
  LL_USART_ConfigAsyncMode(USARTx);
  
  /*Enable USART */
  LL_USART_Enable(USARTx);
}

/**
  * @brief  Uart receive timeout check.
  * @param  None.
  * @retval None
  */
static void APP_UartRxTimeOut(void)
{
  /* Received an idle frame and turned on the check to see
     if "single frame data reception ended" */
  if (CheckFlag == 1)
  {
    /* If the 'timeout time' is exceeded,
       it is considered that receiving one frame of data has ended */
    if (LL_SYSTICK_IsActiveCounterFlag() != 0U)
    {
      timeout --;
    }
    if (timeout == 0U)
    {
      /* Send back received data */      
      APP_UsartTransmit(USART1, (uint8_t *)&RxBuf, RxLen);
      
      /* Enable next data reception */
      CheckFlag = 0;
      RxLen = 0;
    }
    
    /* Received new data, continue receiving data */
    if (CheckLen != RxLen)
    {
      CheckFlag = 0;
    }
  }
}

/**
  * @brief  USART transmission function
  * @param  USARTx：USART Instance，This parameter can be one of the following values:USART1、USART2
  * @param  pData：Pointer to transmission buffer
  * @param  Size：Size of transmission buffer
  * @retval None
  */
static void APP_UsartTransmit(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
  TxBuff = pData;
  TxCount = Size;
  
  /* transmit data */
  while (TxCount > 0)
  {
    /* Wait for TXE bit to be set */
    while(LL_USART_IsActiveFlag_TXE(USARTx) != 1);
    /* transmit data */
    LL_USART_TransmitData8(USARTx, *TxBuff);
    TxBuff++;
    TxCount--;
  }
  
  /* Wait for TC bit to be set */
  while(LL_USART_IsActiveFlag_TC(USARTx) != 1);
}

/**
  * @brief  Error handling function
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
  /* Infinite loop */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file：Pointer to the source file name
  * @param  line：assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add His own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
