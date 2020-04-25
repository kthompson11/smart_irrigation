
#include <stm32f0xx.h>

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
void handle_valves(struct valve_op_request *request);
