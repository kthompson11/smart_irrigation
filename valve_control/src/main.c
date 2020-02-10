#include <stm32f0xx.h>
#include <system_stm32f0xx.h>

void wait(int n_iterations)
{
    volatile int j = 0;

    for (int i = 0; i < n_iterations; ++i)
    {
        j += 1;
    }
}

int main(void)
{
    /* enable IO port C clock */
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    /* set output mode on pin PC8 */
    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER8_Msk, GPIO_MODER_MODER8_0);
    /* set push-pull mode on pin PC8 */
    GPIOC->MODER |= GPIO_OTYPER_OT_8;

    while (1)
    {
        /* toggle output value of PC8 */
        GPIOC->ODR ^= GPIO_ODR_8;

        wait(200000);
    }

    return 0;
}