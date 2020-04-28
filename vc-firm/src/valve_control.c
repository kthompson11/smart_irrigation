/* Handles opening and closing valves based on incoming requests. 
   Only one task using valve_task should be created. */

#include "valve_control.h"
#include "vc-interface.h"
#include "vc-config.h"

/* global state */
static int is_valve_open = 0;
static int current_open_valve = 0;
static TickType_t open_ticks_left = 0;  /* number of ticks until auto shutoff */


static GPIO_TypeDef *get_gpio_base(int valve_number)
{
    GPIO_TypeDef *gpio_base = 0;

    switch (Valve_Ports[valve_number]) {
    case VALVE_PORTA:
        gpio_base = GPIOA;
        break;
    case VALVE_PORTB:
        gpio_base = GPIOB;
        break;
    case VALVE_PORTC:
        gpio_base = GPIOC;
        break;
    case VALVE_PORTD:
        gpio_base = GPIOD;
        break;
    case VALVE_PORTF:
        gpio_base = GPIOF;
        break;
    }

    return gpio_base;
}

void init_valves(void)
{
    /* enable clocks for valve io ports */
    uint32_t gpio_enabled_clocks = 0;
    for (int i = 0; i < N_VALVES; ++i)
    {
        switch (Valve_Ports[i]) {
        case VALVE_PORTA:
            gpio_enabled_clocks |= RCC_AHBENR_GPIOAEN;
            break;
        case VALVE_PORTB:
            gpio_enabled_clocks |= RCC_AHBENR_GPIOBEN;
            break;
        case VALVE_PORTC:
            gpio_enabled_clocks |= RCC_AHBENR_GPIOCEN;
            break;
        case VALVE_PORTD:
            gpio_enabled_clocks |= RCC_AHBENR_GPIODEN;
            break;
        case VALVE_PORTF:
            gpio_enabled_clocks |= RCC_AHBENR_GPIOFEN;
            break;
        }
    }
    RCC->AHBENR |= gpio_enabled_clocks;

    /* setup pin modes and intial values */
    for (int i = 0; i < N_VALVES; ++i) {
        /* get the base address of the gpio port of pin i */
        GPIO_TypeDef *current_gpio_base = get_gpio_base(i);
        
        /* set pin i to output mode */
        const int pin_number = Valve_Pins[i];
        uint32_t clear_mask = 0b11 << (2 * pin_number);
        uint32_t set_mask = 0b01 << (2 * pin_number);
        MODIFY_REG(current_gpio_base->MODER, clear_mask, set_mask);

        /* set pin i to push-pull */
        clear_mask = GPIO_OTYPER_OT_0 << pin_number;
        CLEAR_BIT(current_gpio_base->OTYPER, clear_mask);

        /* set pin i value to 0 */
        clear_mask = GPIO_ODR_0 << pin_number;
        CLEAR_BIT(current_gpio_base->ODR, clear_mask);
    }
}

void close_all_valves(void)
{
    for (int i = 0; i < N_VALVES; ++i) {
        GPIO_TypeDef *current_gpio_base = get_gpio_base(i);
        int pin_number = Valve_Pins[i];
        CLEAR_BIT(current_gpio_base->ODR, (GPIO_ODR_0 << pin_number));
    }

    is_valve_open = 0;
}

void close_valve(void)
{
    GPIO_TypeDef *current_gpio_base = get_gpio_base(current_open_valve);
    int pin_number = Valve_Pins[current_open_valve];

    CLEAR_BIT(current_gpio_base->ODR, (GPIO_ODR_0 << pin_number));

    is_valve_open = 0;
}

void open_valve(uint8_t valve_number)
{
    if (current_open_valve != valve_number) { close_valve(); }

    if (valve_number >= N_VALVES) { return; }

    GPIO_TypeDef *current_gpio_base = get_gpio_base(valve_number);
    int pin_number = Valve_Pins[valve_number];

    current_gpio_base->ODR |= GPIO_ODR_0 << pin_number;

    open_ticks_left = pdMS_TO_TICKS(MAX_OPEN_MS_COUNT);
    is_valve_open = 1;
    current_open_valve = valve_number;
}

void handle_request(uint8_t request)
{
    /* decode request */
    uint8_t opcode = vc_get_opcode(request);
    uint8_t arg = vc_get_arg(request);

    switch (opcode) {
    case VC_OPCODE_OPEN:
        open_valve(arg);
        break;
    case VC_OPCODE_CLOSE:
        close_valve();
        break;
    default:
        /* unrecognized opcode; take no action */
        break;
    }
}

void valve_task(void *param) 
{
    struct valve_task_data *data = (struct valve_task_data*)param;
    QueueHandle_t req_handle = data->req_handle;
    uint8_t request;

    for (;;) {
        BaseType_t status;
        int receive_timed_out;

        /* wait for request or timeout */
        if (is_valve_open) {
            status = xQueueReceive(req_handle, &request, open_ticks_left);
        } else {
            status = xQueueReceive(req_handle, &request, portMAX_DELAY);
        }

        /* get reason for waking */
        receive_timed_out = (status == errQUEUE_EMPTY);

        if (receive_timed_out) {
            close_all_valves();
            is_valve_open = 0;
        } else {
            handle_request(request);
        }
    }
}