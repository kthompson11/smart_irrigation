/* Driver-side header for the driver-microcontroller interface. */

#ifndef VALVE_MESSAGING_H
#define VALVE_MESSAGING_H

/* 
 * valve_control message format
 *  |---- arg (5 bits) ----|---- opcode (3 bits) ----|
 * lsb                                              msb
 */
#define VALVE_OPCODE_LEN 3
#define VALVE_ARG_LEN 5

#define VALVE_OPCODE_OPEN  0x00
#define VALVE_OPCODE_CLOSE 0x01

#define VC_MAKE_OP(opcode, arg) ((arg << VALVE_OPCODE_LEN) | opcode)

#endif  /* VALVE_MESSAGING_H */
