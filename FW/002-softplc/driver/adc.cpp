#include "main.h"
#include "hbox.h"
static ADC_HandleTypeDef             hadc= {0};
static ADC_ChannelConfTypeDef        sConfig= {0};


static void adc_init()
{
    {
        //初始化ADC
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        __HAL_RCC_PWR_CLK_ENABLE();
        __HAL_RCC_ADC_CLK_ENABLE();                                                /* Enable ADC clock */
        hadc.Instance = ADC1;
        hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV4;                /* Set ADC clock*/
        hadc.Init.Resolution            = ADC_RESOLUTION_12B;                      /* 12-bit resolution for converted data */
        hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;                     /* Right-alignment for converted data */
        hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;              /* Scan sequence direction: forward */
        hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;                     /* Single Conversion */
        hadc.Init.LowPowerAutoWait      = DISABLE;                                 /* Auto-delayed conversion feature disabled */
        hadc.Init.ContinuousConvMode    = DISABLE;                                 /* Continuous mode disabled to have only 1 conversion at each conversion trig */
        hadc.Init.DiscontinuousConvMode = DISABLE;                                 /* Disable discontinuous mode */
        hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;                      /* Software start to trig the 1st conversion manually, without external event */
        hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;           /* Parameter discarded because software trigger chosen */
        hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;                /* DR register is overwritten with the last conversion result in case of overrun */
        hadc.Init.SamplingTimeCommon    = ADC_SAMPLETIME_239CYCLES_5;              /* The channel sampling time is 239.5 ADC clock cycles */
        HAL_ADC_Init(&hadc);                                         /* ADC initialization */
        HAL_ADCEx_Calibration_Start(&hadc);                           /* ADC Calibration */
    }
    {
        //配置引脚
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        {
            //AIN3 PA6
            GPIO_InitStruct.Pin = GPIO_PIN_6;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        }
        {
            //AIN4 PA7
            GPIO_InitStruct.Pin = GPIO_PIN_7;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        }
        {
            //AIN7 PB0
            GPIO_InitStruct.Pin = GPIO_PIN_0;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        }
        {
            //AIN0 PB1
            GPIO_InitStruct.Pin = GPIO_PIN_1;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        }

    }

};
extern "C" uint16_t ADCConvert(uint8_t channel)
{
#ifdef ADC_CHANNEL_10
    channel%=11;
#else
    channel%=10;
#endif
    uint32_t Channel=ADC_CHANNEL_0+channel;
    uint16_t adcvalue = 0;
    /* Configure channel need ADC is disable */
    if(READ_BIT(hadc.Instance->CR, ADC_CR_ADEN) == ADC_CR_ADEN)
    {
        __HAL_ADC_DISABLE(&hadc);
    }
    /* Config selected channels */
    sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
    sConfig.Channel      = Channel;
    HAL_ADC_ConfigChannel(&hadc, &sConfig);                       /* Configure ADC Channel */
    /* ADC Start */
    HAL_ADC_Start(&hadc);
    switch(Channel)
    {
    case ADC_CHANNEL_TEMPSENSOR:
    {
        *((__IO uint32_t*)(((uintptr_t)ADC1)+0x308)) |= ADC_CCR_TSEN;
        HAL_Delay(2);
    }
    break;
    case ADC_CHANNEL_VREFINT:
    {
        *((__IO uint32_t*)(((uintptr_t)ADC1)+0x308)) |= ADC_CCR_VREFEN;
        HAL_Delay(2);
    }
    break;
    default:
        break;
    }
    /* Polling for ADC Conversion */
    HAL_ADC_PollForConversion(&hadc, 1000000);
    /* Get ADC Value */
    adcvalue = HAL_ADC_GetValue(&hadc);
    /* Disable ADC to clear channel configuration */
    __HAL_ADC_DISABLE(&hadc);
    /* Clear the selected channels */
    sConfig.Rank         = ADC_RANK_NONE;
    sConfig.Channel      = Channel;
    HAL_ADC_ConfigChannel(&hadc, &sConfig);                      /* Configure ADC Channel */
    return adcvalue;
}

HSTACKLESSCOROUTINE_BLOCK_START(adc)
adc_init();
HSTACKLESSCOROUTINE_BLOCK_END(adc)
