#include <stm32f0xx.h>
#include <system_stm32f0xx.h>

#include "system_timer.h"
#include "valve_control.h"
#include "constants.h"
#include "spi.h"

/* FreeRTOS headers */
#include <FreeRTOS.h>
#include <task.h>

static void setup_hardware(void)
{
    init_valves();
    spi1_init();
}

int main(void)
{
    setup_hardware();

    /* create queue */
    QueueHandle_t req_queue = xQueueCreate(1, 1);

    /* create tasks */
    struct spi_task_data spi_data = {
        .req_out_handle = req_queue,
    };
    xTaskCreate(spi_task, "spi1", configMINIMAL_STACK_SIZE, &spi_data, 1, NULL);

    struct valve_task_data valve_data = {
        .req_handle = req_queue,
    };
    xTaskCreate(valve_task, "valve", configMINIMAL_STACK_SIZE, &valve_data, 2, NULL);

    /* start running tasks */
    vTaskStartScheduler();

    return 0;
}
