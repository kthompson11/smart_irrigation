/* Creates a spi read task that uses a queue to 
   pass incoming requests to another task */

#include <stm32f0xx.h>

#include "spi.h"
#include "vc-interface.h"

static int bytes_received = 0;

/**
 * Synchronizes the SPI data frame
 */
void start_spi_data_frame(void) {
    bytes_received = 0;
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

    /* enable interrupts on CS pin */
    /* set NVIC registers */
    NVIC_EnableIRQ(7);
    /* set EXTI registers */
    SET_BIT(EXTI->IMR, EXTI_IMR_IM4);  /* enable external interrupt 4 */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_FT4);  /* set interrupt for falling edge */
    MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR2_EXTI4_Msk, SYSCFG_EXTICR2_EXTI4_PA);
}

void spi_task(void *param)
{
    struct spi_task_data *data = (struct spi_task_data*)param;
    req_type response, request;
    const int rx_size = sizeof(req_type);
    uint8_t rx_buf[rx_size];

    /* continuously poll for incoming requests */
    /* TODO: switch from polling to interrupts */
    for (;;) {
        /******************** handle rx data ********************/
        int is_rx_pending = SPI1->SR & SPI_SR_RXNE;
        if (is_rx_pending) {
                rx_buf[bytes_received] = SPI1->DR;
                bytes_received += 1;
        }
        
        /* send request when all bytes received */
        if (bytes_received == rx_size) {
            request = *((req_type *)rx_buf);
            bytes_received = 0;
            xQueueSend(data->mosi_queue, &request, portMAX_DELAY);
        }


        /******************** handle tx data ********************/
        /* discard response data for now */
        /* TODO: respond to request */
        xQueueReceive(data->miso_queue, &response, 0);
    }
}

void EXTI4_15_IRQHandler(void)
{
    /* get pending interrupt status */
    uint32_t pr = EXTI->PR;

    /* determine cause of interrupt */
    if (pr & EXTI_PR_PIF4) {
        start_spi_data_frame();
        SET_BIT(EXTI->PR, EXTI_PR_PIF4);
    }
}