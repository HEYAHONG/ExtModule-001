/**
  ******************************************************************************
  * @file    py32f002b_ll_gpio.c
  * @author  MCU Application Team
  * @brief   GPIO LL module driver.
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
#if defined(USE_FULL_LL_DRIVER)

/* Includes ------------------------------------------------------------------*/
#include "py32f002b_ll_gpio.h"
#include "py32f002b_ll_bus.h"
#ifdef  USE_FULL_ASSERT
  #include "py32_assert.h"
#else
  #define assert_param(expr) ((void)0U)
#endif

/** @addtogroup PY32F002B_LL_Driver
  * @{
  */

#if defined (GPIOA) || defined (GPIOB) || defined (GPIOC)

/** @addtogroup GPIO_LL
  * @{
  */
/** MISRA C:2012 deviation rule has been granted for following rules:
  * Rule-12.2 - Medium: RHS argument is in interval [0,INF] which is out of
  * range of the shift operator in following API :
  * LL_GPIO_Init
  * LL_GPIO_DeInit
  * LL_GPIO_SetPinMode
  * LL_GPIO_GetPinMode
  * LL_GPIO_SetPinSpeed
  * LL_GPIO_GetPinSpeed
  * LL_GPIO_SetPinPull
  * LL_GPIO_GetPinPull
  * LL_GPIO_GetAFPin_0_7
  * LL_GPIO_SetAFPin_0_7
  * LL_GPIO_SetAFPin_8_15
  * LL_GPIO_GetAFPin_8_15
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/** @addtogroup GPIO_LL_Private_Macros
  * @{
  */
#define IS_LL_GPIO_PIN(__VALUE__)          (((0x00u) < (__VALUE__)) && ((__VALUE__) <= (LL_GPIO_PIN_ALL)))

#define IS_LL_GPIO_MODE(__VALUE__)         (((__VALUE__) == LL_GPIO_MODE_INPUT)     ||\
                                            ((__VALUE__) == LL_GPIO_MODE_OUTPUT)    ||\
                                            ((__VALUE__) == LL_GPIO_MODE_ALTERNATE) ||\
                                            ((__VALUE__) == LL_GPIO_MODE_ANALOG))

#define IS_LL_GPIO_OUTPUT_TYPE(__VALUE__)  (((__VALUE__) == LL_GPIO_OUTPUT_PUSHPULL)  ||\
                                            ((__VALUE__) == LL_GPIO_OUTPUT_OPENDRAIN))

#define IS_LL_GPIO_SPEED(__VALUE__)        (((__VALUE__) == LL_GPIO_SPEED_FREQ_LOW)       ||\
                                            ((__VALUE__) == LL_GPIO_SPEED_FREQ_MEDIUM)    ||\
                                            ((__VALUE__) == LL_GPIO_SPEED_FREQ_HIGH)      ||\
                                            ((__VALUE__) == LL_GPIO_SPEED_FREQ_VERY_HIGH))

#define IS_LL_GPIO_PULL(__VALUE__)         (((__VALUE__) == LL_GPIO_PULL_NO)   ||\
                                            ((__VALUE__) == LL_GPIO_PULL_UP)   ||\
                                            ((__VALUE__) == LL_GPIO_PULL_DOWN))

#define IS_LL_GPIO_ALTERNATE(__VALUE__)    (((__VALUE__) == LL_GPIO_AF_0  )   ||\
                                            ((__VALUE__) == LL_GPIO_AF_1  )   ||\
                                            ((__VALUE__) == LL_GPIO_AF_2  )   ||\
                                            ((__VALUE__) == LL_GPIO_AF_3  )   ||\
                                            ((__VALUE__) == LL_GPIO_AF_4  )   ||\
                                            ((__VALUE__) == LL_GPIO_AF_5  )   ||\
                                            ((__VALUE__) == LL_GPIO_AF_6  )   ||\
                                            ((__VALUE__) == LL_GPIO_AF_7  ))
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @addtogroup GPIO_LL_Exported_Functions
  * @{
  */

/** @addtogroup GPIO_LL_EF_Init
  * @{
  */

/**
  * @brief  De-initialize GPIO registers (Registers restored to their default values).
  * @param  GPIOx GPIO Port
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: GPIO registers are de-initialized
  *          - ERROR:   Wrong GPIO Port
  */
ErrorStatus LL_GPIO_DeInit(GPIO_TypeDef *GPIOx)
{
  ErrorStatus status = SUCCESS;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));

  /* Force and Release reset on clock of GPIOx Port */
  if (GPIOx == GPIOA)
  {
    LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_GPIOA);
  }
  else if (GPIOx == GPIOB)
  {
    LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_GPIOB);
  }
  else if (GPIOx == GPIOC)
  {
    LL_IOP_GRP1_ForceReset(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_IOP_GRP1_ReleaseReset(LL_IOP_GRP1_PERIPH_GPIOC);
  }
  else
  {
    status = ERROR;
  }

  return (status);
}

/**
  * @brief  Initialize GPIO registers according to the specified parameters in GPIO_InitStruct.
  * @param  GPIOx GPIO Port
  * @param  GPIO_InitStruct pointer to a @ref LL_GPIO_InitTypeDef structure
  *         that contains the configuration information for the specified GPIO peripheral.
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: GPIO registers are initialized according to GPIO_InitStruct content
  *          - ERROR:   Not applicable
  */
ErrorStatus LL_GPIO_Init(GPIO_TypeDef *GPIOx, LL_GPIO_InitTypeDef *GPIO_InitStruct)
{
  uint32_t pinpos;
  uint32_t currentpin;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
  assert_param(IS_LL_GPIO_PIN(GPIO_InitStruct->Pin));
  assert_param(IS_LL_GPIO_MODE(GPIO_InitStruct->Mode));
  assert_param(IS_LL_GPIO_PULL(GPIO_InitStruct->Pull));

  /* ------------------------- Configure the port pins ---------------- */
  /* Initialize  pinpos on first pin set */
  pinpos = 0;

  /* Configure the port pins */
  while (((GPIO_InitStruct->Pin) >> pinpos) != 0x00u)
  {
    /* Get current io position */
    currentpin = (GPIO_InitStruct->Pin) & (0x00000001uL << pinpos);

    if (currentpin != 0x00u)
    {
      /* Pin Mode configuration */
      LL_GPIO_SetPinMode(GPIOx, currentpin, GPIO_InitStruct->Mode);

      if ((GPIO_InitStruct->Mode == LL_GPIO_MODE_OUTPUT) || (GPIO_InitStruct->Mode == LL_GPIO_MODE_ALTERNATE))
      {
        /* Check Speed mode parameters */
        assert_param(IS_LL_GPIO_SPEED(GPIO_InitStruct->Speed));

        /* Speed mode configuration */
        LL_GPIO_SetPinSpeed(GPIOx, currentpin, GPIO_InitStruct->Speed);
      }

      /* Pull-up Pull down resistor configuration*/
      LL_GPIO_SetPinPull(GPIOx, currentpin, GPIO_InitStruct->Pull);

      if (GPIO_InitStruct->Mode == LL_GPIO_MODE_ALTERNATE)
      {
        /* Check Alternate parameter */
        assert_param(IS_LL_GPIO_ALTERNATE(GPIO_InitStruct->Alternate));

        /* Speed mode configuration */
        if (currentpin < LL_GPIO_PIN_8)
        {
          LL_GPIO_SetAFPin_0_7(GPIOx, currentpin, GPIO_InitStruct->Alternate);
        }
        else
        {
          LL_GPIO_SetAFPin_8_15(GPIOx, currentpin, GPIO_InitStruct->Alternate);
        }
      }
    }
    pinpos++;
  }

  if ((GPIO_InitStruct->Mode == LL_GPIO_MODE_OUTPUT) || (GPIO_InitStruct->Mode == LL_GPIO_MODE_ALTERNATE))
  {
    /* Check Output mode parameters */
    assert_param(IS_LL_GPIO_OUTPUT_TYPE(GPIO_InitStruct->OutputType));

    /* Output mode configuration*/
    LL_GPIO_SetPinOutputType(GPIOx, GPIO_InitStruct->Pin, GPIO_InitStruct->OutputType);

  }
  return (SUCCESS);
}

/**
  * @brief Set each @ref LL_GPIO_InitTypeDef field to default value.
  * @param GPIO_InitStruct pointer to a @ref LL_GPIO_InitTypeDef structure
  *        whose fields will be set to default values.
  * @retval None
  */

void LL_GPIO_StructInit(LL_GPIO_InitTypeDef *GPIO_InitStruct)
{
  /* Reset GPIO init structure parameters values */
  GPIO_InitStruct->Pin        = LL_GPIO_PIN_ALL;
  GPIO_InitStruct->Mode       = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct->Speed      = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct->OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct->Pull       = LL_GPIO_PULL_NO;
  GPIO_InitStruct->Alternate  = LL_GPIO_AF_0;
}

/**
  * @}
  */

/**
  * @}
  */



/**
  * @}
  */

#endif /* defined (GPIOA) || defined (GPIOB) || defined (GPIOC) */
/**
  * @}
  */

#endif /* USE_FULL_LL_DRIVER */

/************************ (C) COPYRIGHT Puya Semiconductor *****END OF FILE****/
