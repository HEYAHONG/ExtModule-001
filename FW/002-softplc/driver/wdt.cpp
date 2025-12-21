#include "main.h"
#include "hbox.h"

IWDG_HandleTypeDef   IwdgHandle= {0};
enum
{
    LABEL_WATCHDOG_FEED=1,
};
static void hw_feed()
{
    HAL_IWDG_Refresh(&IwdgHandle);
};
static void sys_reset()
{
    NVIC_SystemReset();
}
static hwatchdog_tick_t sys_tick_ms()
{
    return hdefaults_tick_get();
};

HSTACKLESSCOROUTINE_BLOCK_START(wdt)
hwatchdog_set_hardware_dog_feed(hw_feed);
hwatchdog_setup_software_dog(sys_reset,sys_tick_ms);
IwdgHandle.Instance = IWDG;                     /* IWDG */
IwdgHandle.Init.Prescaler = IWDG_PRESCALER_32;  /* Prescaler DIV 32 */
IwdgHandle.Init.Reload = (1024);                /* IWDG Reload value 1024 */
HAL_IWDG_Init(&IwdgHandle);
__HAL_DBGMCU_FREEZE_IWDG(); /* can be removed in the product */
hstacklesscoroutine_yield_with_label(LABEL_WATCHDOG_FEED);
HWATCHDOG_FEED();
hstacklesscoroutine_goto_label(LABEL_WATCHDOG_FEED);
HSTACKLESSCOROUTINE_BLOCK_END(wdt)
