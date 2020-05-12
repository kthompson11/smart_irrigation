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

/* TODO: move these defines out of interface header */
/* vc-firm configuration constants */
#define N_VALVES 2
/* number of seconds must fit in req_type (VC_OPCODE_OPENTIME) */
#define MAX_OPEN_MS_COUNT (60 * 1000)

typedef uint16_t req_type; /* type used to represent a request */
typedef uint16_t crc_type; /* type used for CRC checking */

/* 
 * valve_control request format
 *  |---- arg (11 bits) ----|---- opcode (5 bits) ----|
 * msb                                              lsb
 */
#define VALVE_OPCODE_LEN    5
#define VALVE_ARG_LEN       11
#define VALVE_OPCODE_MASK   0x1F

#define VC_OPCODE_OPEN      0x00
#define VC_OPCODE_CLOSE     0x01
#define VC_OPCODE_OPENTIME  0x02
#define VC_OPCODE_NVALVES   0x03

static inline req_type vc_make_request(req_type opcode, req_type arg)
{
    return ((arg << VALVE_OPCODE_LEN) | opcode);
}

static inline req_type vc_get_opcode(req_type request)
{
    return (request & VALVE_OPCODE_MASK);
}

static inline req_type vc_get_arg(req_type request)
{
    return (request >> VALVE_OPCODE_LEN);
}

/**
 * Checks if a request is valid
 * 
 * @return 1 if request is valid; 0 otherwise
 */
static inline int vc_is_request_valid(req_type request)
{
    if (vc_get_arg(request) >= N_VALVES) {
        return 0;
    }

    return 1;
}

#endif  /* VC_INTERFACE_H */
