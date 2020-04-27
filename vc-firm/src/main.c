#include <stm32f0xx.h>
#include <system_stm32f0xx.h>

#include "system_timer.h"
#include "valve_control.h"
#include "constants.h"

/* FreeRTOS headers */
#include <FreeRTOS.h>
#include <task.h>



void wait(int n_iterations)
{
    volatile int j = 0;

    for (int i = 0; i < n_iterations; ++i)
    {
        j +=1;
    }
}

void spi1_init(void)
{
    /* enable SPI1 clock */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    /* configure SPI1 pins */
    /* TODO: define which pins to use for SPI1 elsewhere */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER4_Msk, GPIO_MODER_MODER4_1);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER5_Msk, GPIO_MODER_MODER5_1);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER6_Msk, GPIO_MODER_MODER6_1);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER7_Msk, GPIO_MODER_MODER7_1);
    /* choose alternate functions */
    /* (default after reset is correct for SPI1) */

    /* set CR1 register fields */
    CLEAR_BIT(SPI1->CR1, SPI_CR1_CPOL);
    CLEAR_BIT(SPI1->CR1, SPI_CR1_CPHA);
    CLEAR_BIT(SPI1->CR1, SPI_CR1_RXONLY);
    CLEAR_BIT(SPI1->CR1, SPI_CR1_BIDIMODE);
    CLEAR_BIT(SPI1->CR1, SPI_CR1_LSBFIRST);
    CLEAR_BIT(SPI1->CR1, SPI_CR1_CRCEN);
    CLEAR_BIT(SPI1->CR1, SPI_CR1_MSTR);
    CLEAR_BIT(SPI1->CR1, SPI_CR1_SSM);

    /* set CR2 register fields */
    const uint32_t data_size_8_bit = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
    MODIFY_REG(SPI1->CR2, SPI_CR2_DS_Msk, data_size_8_bit);
    SET_BIT(SPI1->CR2, SPI_CR2_FRXTH);  /* 8 bit threshold on RXFIFO */

    /* enable SPI1 */
    SET_BIT(SPI1->CR1, SPI_CR1_SPE);
}

static void setup_hardware(void)
{
    /*setup_hardware();*/
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	GPIOC->MODER |= (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0) ;
	/* Configure PC8 and PC9 in output  mode  */

	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8 | ~GPIO_OTYPER_OT_9) ;
	// Ensure push pull mode selected--default

	GPIOC->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR9);
	//Ensure maximum speed setting (even though it is unnecessary)

	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR9);
	//Ensure all pull up pull down resistors are disabled
}

struct blink_data {
    uint32_t pin;
    TickType_t interval;
};

static void blinkTask(void *param)
{
    uint32_t pin = ((struct blink_data*)param)->pin;
    TickType_t interval = ((struct blink_data*)param)->interval;

    for (;;)
    {
        GPIOC->ODR ^= pin;

        vTaskDelay(interval);
    }
}

int main(void)
{
    struct blink_data bdata8 = {
        .pin = GPIO_ODR_8,
        .interval = 1000,
    };
    struct blink_data bdata9 = {
        .pin = GPIO_ODR_9,
        .interval = 500,
    };

    setup_hardware();

    /* create tasks */
    xTaskCreate(blinkTask, "blink8", configMINIMAL_STACK_SIZE, &bdata8, 2, NULL);
    xTaskCreate(blinkTask, "blink9", configMINIMAL_STACK_SIZE, &bdata9, 2, NULL);

    /* start running tasks */
    vTaskStartScheduler();


    #ifdef COMMENTED_OUT
    /* initialize the system timer (SysTick) */
    init_system_timer();

    /* initialize SPI1 */
    spi1_init();

    /* initialize valves */
    init_valves();

    struct valve_op_request spi_op;
    spi_op.is_op_pending = 0;

    while (1)
    {
        /* TODO: wait for ms instead of looping */
        wait(10000);

        /* spi tx/rx */
        int is_rx_pending = SPI1->SR & SPI_SR_RXNE;
        if (is_rx_pending)
        {
            spi_op.is_op_pending = 1;
            spi_op.op = SPI1->DR;
        }

        handle_valves(&spi_op);
    }
    #endif

    return 0;
}
