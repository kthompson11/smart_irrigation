/* contains a request that handles incoming requests */

#include "decode.h"
#include "vc-interface.h"

void decode_task(void *param)
{
    struct decode_task_data *data = (struct decode_task_data *)param;
    req_type request, response;
    
    for (;;) {
        xQueueReceive(data->spi_mosi_queue, &request, portMAX_DELAY);

        /* request will always contain valid data */
        switch (vc_get_opcode(request)) {
        case VC_OPCODE_OPEN:
        case VC_OPCODE_CLOSE:
            /* forward request to valve_control task */
            response = 0b0101010111001100;
            /*response = 0;*/
            xQueueSend(data->spi_miso_queue, &response, portMAX_DELAY);
            xQueueSend(data->valve_control_queue, &request, portMAX_DELAY);
            break;
        case VC_OPCODE_OPENTIME:
            /* return number of seconds */
            response = (MAX_OPEN_MS_COUNT / 1000);
            xQueueSend(data->spi_miso_queue, &response, portMAX_DELAY);
            break;
        case VC_OPCODE_NVALVES:
            response = N_VALVES;
            xQueueSend(data->spi_miso_queue, &response, portMAX_DELAY);
            break;
        default:
            response = -VC_EREQ;
            xQueueSend(data->spi_miso_queue, &response, portMAX_DELAY);
            break;
        }
    }
}