#ifndef VALVE_CONTROL_H
#define VALVE_CONTROL_H

#include <stm32f0xx.h>

#include <FreeRTOS.h>
#include <queue.h>

#include "constants.h"

#define N_VALVES 2
#define MAX_OPEN_MS_COUNT (60 * 1000)

enum valve_ioport { VALVE_PORTA, VALVE_PORTB, VALVE_PORTC,
                    VALVE_PORTD, VALVE_PORTF };

static const enum valve_ioport Valve_Ports[N_VALVES] = { VALVE_PORTC,
                                                         VALVE_PORTC };
static const int Valve_Pins[N_VALVES] = { 13,
                                          14 };

void init_valves(void);

struct valve_task_data {
    QueueHandle_t req_handle;
};

void valve_task(void *param);

#endif /* VALVE_CONTROL_H */