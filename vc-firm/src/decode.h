#ifndef DECODE_H
#define DECODE_H

#include <FreeRTOS.h>
#include <queue.h>

struct decode_task_data {
    QueueHandle_t spi_mosi_queue;
    QueueHandle_t spi_miso_queue;
    QueueHandle_t valve_control_queue;
};

/**
 * FreeRTOS task that decodes and responds to requests
 * 
 * @param param decode_task_data* with valid fields
 */
void decode_task(void *param);

#endif /* DECODE_H */