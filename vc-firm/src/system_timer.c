#include <stm32f0xx.h>

#include "system_timer.h"

static struct systick_ms ms_count;

void init_system_timer(void)
{
    /* initialize the millisecond timer */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  /* enable SysTick interrupt */
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;  /* use SystemCoreClock / 8 */
    const uint32_t reload_value = SystemCoreClock / 8 / 1000 - 1;  /* 1000 Hz */
    SysTick->LOAD = reload_value;
    ms_count.lower = 0;
    ms_count.upper = 0;
    SysTick->VAL = 0;  /* reset SysTick counter to zero */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  /* enable the SysTick timer */
}

struct systick_ms get_systick_ms_count(void)
{
    return ms_count;
}

/* Compare two systick_ms_count structs and returns 1 if the first struct
 * represents a later time than the second struct */
int systick_gt(struct systick_ms lhs, struct systick_ms rhs)
{
    int res = 0;

    if (lhs.upper > rhs.upper)
    {
        res = 1;
    }
    else if ((lhs.upper == rhs.upper) && (lhs.lower > rhs.lower))
    {
        res = 1;
    }
    
    return res;
}

struct systick_ms systick_ms_add(struct systick_ms lhs, uint32_t rhs)
{
    struct systick_ms res;
    res.lower = lhs.lower + rhs;

    /* check for overflow of lower counter */
    if (lhs.lower > res.lower)
    {
        res.upper = lhs.upper + 1;
    }
    else
    {
        res.upper = lhs.upper;
    }
    
    return res;
}

/*
void SysTick_Handler(void)
{
    ms_count = systick_ms_add(ms_count, 1);
}
*/