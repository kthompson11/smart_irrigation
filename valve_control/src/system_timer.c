#include <stm32f0xx.h>

#include "system_timer.h"

uint32_t systick_ms_count;

void init_system_timer(void)
{
    /* initialize the millisecond timer */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  /* enable SysTick interrupt */
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;  /* use SystemCoreClock / 8 */
    const uint32_t reload_value = SystemCoreClock / 8 / 1000 - 1;  /* 1000 Hz */
    SysTick->LOAD = reload_value;
    systick_ms_count = 0;
    SysTick->VAL = 0;  /* reset SysTick counter to zero */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  /* enable the SysTick timer */
}

void SysTick_Handler(void)
{
    systick_ms_count += 1;
}