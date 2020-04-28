#ifndef VALVE_CONTROL_H
#define VALVE_CONTROL_H

#include <FreeRTOS.h>
#include <queue.h>

void init_valves(void);

struct valve_task_data {
    QueueHandle_t req_handle;
};

void valve_task(void *param);

#endif /* VALVE_CONTROL_H */