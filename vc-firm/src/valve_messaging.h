/* This file provides functions needed to construct and decode 
 * requests sent over SPI to the valve control unit */

#ifndef VALVE_MESSAGING_H
#define VALVE_MESSAGING_H

#include <stdint.h>

enum valve_op
{
    VALVE_OPEN,
    VALVE_CLOSE,
    VALVE_INVALID_OP
};

/* 
 * valve_control message format
 *  |---- arg (5 bits) ----|---- opcode (3 bits) ----|
 * lsb                                              msb
 */
#define VALVE_OPCODE_LEN 3
#define VALVE_ARG_LEN 5

#define VALVE_OPCODE_OPEN  0x00
#define VALVE_OPCODE_CLOSE 0x01

enum valve_op decode_opcode(uint8_t request);
uint8_t decode_arg(uint8_t request);

#endif  /* VALVE_MESSAGING_H */