/* driver-microcontroller interface header */

#ifndef VALVE_MESSAGING_H
#define VALVE_MESSAGING_H

#ifdef __KERNEL__
    #define uint8_t u8
#else
#include <stdint.h>
#endif /* __KERNEL__

/* 
 * valve_control request format
 *  |---- arg (5 bits) ----|---- opcode (3 bits) ----|
 * msb                                              lsb
 */
#define VALVE_OPCODE_LEN 3
#define VALVE_ARG_LEN 5
#define VALVE_OPCODE_MASK 0x07

#define VALVE_OPCODE_OPEN  0x00
#define VALVE_OPCODE_CLOSE 0x01


static inline uint8_t vc_make_request(opcode, arg)
{
    return ((arg << VALVE_OPCODE_LEN) | opcode);
}

static inline uint8_t vc_get_opcode(request)
{
    return (request >> VALVE_OPCODE_LEN);
}

static inline uint8_t vc_get_arg(request)
{
    return (request & VALVE_OPCODE_MASK);
}

#endif  /* VALVE_MESSAGING_H */
