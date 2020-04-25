
#include "valve_control.h"
#include "system_timer.h"
#include "valve_messaging.h"

static struct systick_ms auto_shutoff_ms;

static int is_valve_open = 0;
static int current_open_valve = 0;

static GPIO_TypeDef *get_gpio_base(int valve_number)
{
    GPIO_TypeDef *gpio_base = 0;

    switch (Valve_Ports[valve_number])
    {
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
        switch (Valve_Ports[i])
        {
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
    for (int i = 0; i < N_VALVES; ++i)
    {
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
    for (int i = 0; i < N_VALVES; ++i)
    {
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

void open_valve(int valve_number)
{
    if (current_open_valve != valve_number) { close_valve(); }

    GPIO_TypeDef *current_gpio_base = get_gpio_base(valve_number);
    int pin_number = Valve_Pins[valve_number];

    current_gpio_base->ODR |= GPIO_ODR_0 << pin_number;

    auto_shutoff_ms = systick_ms_add(get_systick_ms_count(), MAX_OPEN_MS_COUNT);
    is_valve_open = 1;
    current_open_valve = valve_number;
}

void handle_valves(struct valve_op_request *request)
{
    if (request->is_op_pending)
    {
        /* clear request */
        request->is_op_pending = 0;

        /* decode the op */
        enum valve_op op = decode_opcode(request->op);
        uint8_t arg = decode_arg(request->op);

        /* handle the op */
        switch (op)
        {
        case VALVE_OPEN:
            open_valve(arg);
            break;
        case VALVE_CLOSE:
            close_valve();
            break;
        default:
            /* unrecognized operation; take no action */
            break;
        }
    }

    /* check if valves should automatically close */
    const struct systick_ms ms_count = get_systick_ms_count();
    if (systick_gt(ms_count, auto_shutoff_ms))  /* ms_count > auto_shutoff_ms */
    {
        close_all_valves();
    }
}
