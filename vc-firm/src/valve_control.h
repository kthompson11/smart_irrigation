#ifndef VALVE_CONTROL_H
#define VALVE_CONTROL_H

#include <FreeRTOS.h>
#include <queue.h>

/**
 * Sets up GPIOs for valves
 */
void init_valves(void);

struct valve_task_data {
    QueueHandle_t req_handle;  /* handle to receive requests from */
};

/**
 * FreeRTOS task that reads requests and handles the valves
 * 
 * @param param struct valve_task_data* with valid fields
 */
void valve_task(void *param);

#endif /* VALVE_CONTROL_H */