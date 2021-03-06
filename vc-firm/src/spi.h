#ifndef SPI_H
#define SPI_H

#include <FreeRTOS.h>
#include <queue.h>

struct spi_task_data {
    QueueHandle_t mosi_queue;
    QueueHandle_t miso_queue;
};

/**
 * Sets up SPI1 in the required mode
 */
void spi1_init(void);

/**
 * FreeRTOS task that forwards bytes read from SPI
 * 
 * @param param struct spi_task_data* with valid fields
 */
void spi_task(void *param);

#endif /* SPI_H */