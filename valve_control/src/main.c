#include <stm32f0xx.h>
#include <system_stm32f0xx.h>

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

int main(void)
{
    spi1_init();

    /* enable GPIOC clock */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    /* set pin PC9 to output */
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER8_1);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER8_0);
    /* set PC9 to push-pull */
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_8);

    GPIOC->ODR |= GPIO_ODR_8;

    while (1)
    {
        wait(10000);

        /* poll the status of the rx buffer */
        volatile int is_rx_data_available = SPI1->SR & SPI_SR_RXNE;
        if (is_rx_data_available)
        {
            volatile uint8_t x = SPI1->DR;
            SPI1->DR = x;

            GPIOC->ODR ^= GPIO_ODR_8;
        }
    }

    return 0;
}