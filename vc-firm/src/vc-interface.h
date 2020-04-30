/** driver-microcontroller interface header **
 *
 * This file contains the interface that the microcontroller
 * presents to the driver.
 */

#ifndef VC_INTERFACE_H
#define VC_INTERFACE_H

#ifndef __KERNEL__
#include <stdint.h>
#endif /* __KERNEL__ */

/* vc-firm configuration constants */
#define N_VALVES 2
#define MAX_OPEN_MS_COUNT (60 * 1000)

/* 
 * valve_control request format
 *  |---- arg (5 bits) ----|---- opcode (3 bits) ----|
 * msb                                              lsb
 */
#define VALVE_OPCODE_LEN    3
#define VALVE_ARG_LEN       5
#define VALVE_OPCODE_MASK   0x07

#define VC_OPCODE_OPEN      0x00
#define VC_OPCODE_CLOSE     0x01

static inline uint8_t vc_make_request(uint8_t opcode, uint8_t arg)
{
    return ((arg << VALVE_OPCODE_LEN) | opcode);
}

static inline uint8_t vc_get_opcode(uint8_t request)
{
    return (request & VALVE_OPCODE_MASK);
}

static inline uint8_t vc_get_arg(uint8_t request)
{
    return (request >> VALVE_OPCODE_LEN);
}

/**
 * Checks if a request is valid
 * 
 * @return 1 if request is valid; 0 otherwise
 */
static inline int vc_is_request_valid(uint8_t request)
{
    if (vc_get_arg(request) >= N_VALVES) {
        return 0;
    }

    return 1;
}

#endif  /* VC_INTERFACE_H */
