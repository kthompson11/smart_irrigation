
#include "valve_messaging.h"

/**
 * Extracts the opcode from a request.
 * request: the request to decode
 * 
 * return: value of request opcode
 */
enum valve_op decode_opcode(uint8_t request)
{
    const uint8_t opcode = request % (1 << VALVE_OPCODE_LEN);

    enum valve_op op;
    switch (opcode)
    {
    case VALVE_OPCODE_OPEN:
        op = VALVE_OPEN;
        break;
    case VALVE_CLOSE:
        op = VALVE_CLOSE;
        break;
    default:
        op = VALVE_INVALID_OP;
        break;
    }

    return op;
}

/**
 * Decodes the argument of a request.
 * request: the request to decode
 * 
 * return: value of request argument
 */
uint8_t decode_arg(uint8_t request)
{
    uint8_t arg = request / (1 << VALVE_OPCODE_LEN);

    return arg;
}

/**
 * Constructs a request from an opcode and argument.
 * op: opcode for the request
 * arg: argument for the request
 * 
 * return: a byte representing the request
 */
uint8_t construct_request(enum valve_op op, uint8_t arg)
{
    /* get opcode */
    uint8_t opcode;
    switch (op)
    {
    case VALVE_OPEN:
        opcode = VALVE_OPCODE_OPEN;
        break;
    case VALVE_CLOSE:
        opcode = VALVE_OPCODE_CLOSE;
        break;
    default:
        /* do nothing; this should never be taken */
        break;
    }

    /* construct request */
    uint8_t request = (arg << VALVE_OPCODE_LEN) | opcode;

    return request;
}
