/* Creates a spi read task that uses a queue to 
   pass incoming requests to another task */

#include <stm32f0xx.h>

#include "spi.h"
#include "vc-interface.h"

static int bytes_received = 0;
enum spi_state {SPI_IDLE, SPI_RX, SPI_TX};
static enum spi_state state = SPI_IDLE;

/**
 * Synchronizes the SPI data frame
 */
void start_spi_data_frame(void) {
    bytes_received = 0;
    state = SPI_RX;
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

    BaseType_t status;
    const int tx_size = sizeof(req_type);
    uint8_t tx_buf[tx_size];
    int tx_bytes_left = 0;
    int tx_bytes_sent = 0;

    /* continuously poll for incoming requests */
    /* TODO: switch from polling to interrupts */
    for (;;) {
        switch (state) {
        case SPI_IDLE:
            /* do nothing */
            /* TODO: wait for interrupt instead of busy waiting */
            /* discard rx data */
            if (SPI1->SR & SPI_SR_RXNE) {
                rx_buf[bytes_received] = SPI1->DR;
            }
            break;
        case SPI_RX:
            /******************** handle rx data ********************/
            /* if spi data available, copy to buffer */
            if (SPI1->SR & SPI_SR_RXNE) {
                rx_buf[bytes_received] = SPI1->DR;
                bytes_received += 1;
            }

            /* send request when all bytes received */
            if (bytes_received == rx_size) {
                request = *((req_type *)rx_buf);
                bytes_received = 0;
                xQueueSend(data->mosi_queue, &request, portMAX_DELAY);
                state = SPI_TX;
            }
            break;
        case SPI_TX:
            /******************** handle tx data ********************/
            /* discard rx data */
            if (SPI1->SR & SPI_SR_RXNE) {
                rx_buf[bytes_received] = SPI1->DR;
            }

            if (tx_bytes_left == 0) {
                /* check for response */
                status = xQueueReceive(data->miso_queue, &response, 0);
                if (status == pdPASS) {
                    tx_bytes_left = sizeof(req_type);
                    tx_bytes_sent = 0;
                    *((req_type *)tx_buf) = response;
                }
            }

            /* put bytes in tx buffer if space is available */
            while ((tx_bytes_left != 0) && (SPI1->SR & SPI_SR_TXE)) {
                *((uint8_t *)(&SPI1->DR)) = tx_buf[tx_bytes_sent];
                ++tx_bytes_sent;
                --tx_bytes_left;

                if (tx_bytes_left == 0) {
                    state = SPI_IDLE;
                }
            }
            break;
        default:
            state = SPI_IDLE;
            break;
        }
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