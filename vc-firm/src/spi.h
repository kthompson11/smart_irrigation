#ifndef SPI_H
#define SPI_H

#include <FreeRTOS.h>
#include <queue.h>

struct spi_task_data {
    QueueHandle_t req_out_handle;
};

void spi1_init(void);
void spi_task(void *param);

#endif /* SPI_H */