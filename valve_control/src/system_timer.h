#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <stdint.h>

void init_system_timer(void);
uint32_t get_systick_ms_count(void);

#endif  /* SYSTEM_TIMER_H */