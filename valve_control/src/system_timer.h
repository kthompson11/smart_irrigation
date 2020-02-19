#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <stdint.h>

/* describes a 64 bit counter representing time elapsed 
 * (in milliseconds) from timer initialization */
struct systick_ms 
{
    uint32_t lower;
    uint32_t upper;
};

void init_system_timer(void);
struct systick_ms get_systick_ms_count(void);
int systick_gt(struct systick_ms lhs, struct systick_ms rhs);
struct systick_ms systick_ms_add(struct systick_ms lhs, uint32_t rhs);

#endif  /* SYSTEM_TIMER_H */