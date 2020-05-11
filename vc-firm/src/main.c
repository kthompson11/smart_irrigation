#include <stm32f0xx.h>

#include "valve_control.h"
#include "spi.h"
#include "decode.h"

/* FreeRTOS headers */
#include <FreeRTOS.h>
#include <task.h>

/**
 * Sets the hardware up for the tasks
 */
static void setup_hardware(void)
{
    init_valves();
    spi1_init();
}

int main(void)
{
    setup_hardware();

    /* create queues */
    QueueHandle_t spi_mosi_queue = xQueueCreate(1, 1);
    QueueHandle_t spi_miso_queue = xQueueCreate(1, 1);
    QueueHandle_t vc_req_queue = xQueueCreate(1, 1);

    /* create tasks */
    struct spi_task_data spi_data = {
        .mosi_queue = spi_mosi_queue,
        .miso_queue = spi_miso_queue,
    };
    xTaskCreate(spi_task, "spi1", configMINIMAL_STACK_SIZE, &spi_data, 1, NULL);

    struct valve_task_data valve_data = {
        .req_handle = vc_req_queue,
    };
    xTaskCreate(valve_task, "valve", configMINIMAL_STACK_SIZE, &valve_data, 2, NULL);

    struct decode_task_data decode_data = {
        .spi_mosi_queue = spi_mosi_queue,
        .spi_miso_queue = spi_miso_queue,
        .valve_control_queue = vc_req_queue,
    };
    xTaskCreate(decode_task, "decode", configMINIMAL_STACK_SIZE, &decode_data, 2, NULL);

    /* start running tasks */
    vTaskStartScheduler();

    return 0;
}
