/* Contains valve port/pin configuration data */

#ifndef VC_CONFIG_H
#define VC_CONFIG_H

#include "vc-interface.h"

enum valve_ioport { VALVE_PORTA, VALVE_PORTB, VALVE_PORTC,
                    VALVE_PORTD, VALVE_PORTF };

static const enum valve_ioport Valve_Ports[N_VALVES] = { VALVE_PORTC,
                                                         VALVE_PORTC };
static const int Valve_Pins[N_VALVES] = { 13,
                                          14 };

#endif /* PIN_CONFIG_H */