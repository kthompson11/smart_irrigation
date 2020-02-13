#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <stdint.h>

extern uint32_t systick_ms_count;

void init_system_timer(void);

#endif  /* SYSTEM_TIMER_H */