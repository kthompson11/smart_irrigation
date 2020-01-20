
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>

#include "bcm2835_spi.h"
#include "bcm2835_constants.h"

static periph_mappings *BCM2835 = NULL;
static int gpioMapped = 0;
static int spi0Mapped = 0;

static int bcm2835_setPinMode(int pinNumber, PinMode mode);
static int bcm2835_setRegister(volatile uint32_t *const regAddr, const uint32_t value, const uint32_t mask);


int bcm2835_spiInit(void)
{
    /* open mem device (/dev/mem) */
    int fd = open(memDevicePath, O_RDWR | O_SYNC);
    if (fd == -1)
    {
        switch (errno)
        {
            case EACCES:
                fprintf(stderr, "Insufficient privileges to open \"%s\".", memDevicePath);
                break;
            default:
                fprintf(stderr, "Unknown error occurred when opening \"%s\".", memDevicePath);
                break;
        }

        bcm2835_spiFree();
        return 0;
    }

    /* map peripherals into virtual address space */
    BCM2835 = malloc(sizeof(periph_mappings));
    if (BCM2835 == NULL)
    {
        fprintf(stderr, "Unknown error occurred when calling malloc().");
        bcm2835_spiFree();
        close(fd);
        return 0;
    }
    const int fileProtections = PROT_READ | PROT_WRITE;
    const int mmapFlags = MAP_SHARED;
    void *mmapResult;

    /* map GPIO base address */
    mmapResult = mmap(NULL, sizeof(gpio_typedef), fileProtections, mmapFlags, fd, BCM2835_GPIO_BASE);
    if (mmapResult == MAP_FAILED)
    {
        fprintf(stderr, "GPIO mmap failed.\n");
        bcm2835_spiFree();
        close(fd);
        return 0;
    }
    else
    {
        gpioMapped = 1;
        BCM2835->GPIO = (volatile gpio_typedef*)mmapResult;
    }
    
    /* map SPI0 base address */
    mmapResult = mmap(NULL, sizeof(spi0_typedef), fileProtections, mmapFlags, fd, BCM2835_SPI0_BASE);
    if (mmapResult == MAP_FAILED)
    {
        fprintf(stderr, "SPI0 mmap failed.\n");
        bcm2835_spiFree();
        close(fd);
        return 0;
    }
    else
    {
        spi0Mapped = 1;
        BCM2835->SPI0 = (volatile spi0_typedef*)mmapResult;
    }

    /* close mem device */
    if (close(fd) == -1)
    {
        fprintf(stderr, "Error occurred on closing mem device file.");
    }

    /* Set gpio pins 8-11 to spi mode */
    __sync_synchronize();
    bcm2835_setPinMode(8, PIN_MODE_ALT0);
    __sync_synchronize();
    bcm2835_setPinMode(9, PIN_MODE_ALT0);
    __sync_synchronize();
    bcm2835_setPinMode(10, PIN_MODE_ALT0);
    __sync_synchronize();
    bcm2835_setPinMode(11, PIN_MODE_ALT0);
    __sync_synchronize();

    /* setup SPI0 control register */
    BCM2835->SPI0->CS = (BCM2835_SPI0_CS_CS_SELECT0 | 
                         BCM2835_SPI0_CS_CLEAR_CLEARTX | 
                         BCM2835_SPI0_CS_CLEAR_CLEARRX);
    __sync_synchronize();

    return 1;
}


int bcm2835_spiFree(void)
{
    /* set gpio pins to default */
    __sync_synchronize();
    bcm2835_setPinMode(8, PIN_MODE_INPUT);
    __sync_synchronize();
    bcm2835_setPinMode(9, PIN_MODE_INPUT);
    __sync_synchronize();
    bcm2835_setPinMode(10, PIN_MODE_INPUT);
    __sync_synchronize();
    bcm2835_setPinMode(11, PIN_MODE_INPUT);
    __sync_synchronize();

    /* unmap peripheral address translations */
    if (gpioMapped) { munmap((void*)(BCM2835->GPIO), sizeof(gpio_typedef)); }
    if (spi0Mapped) { munmap((void*)(BCM2835->SPI0), sizeof(spi0_typedef)); }

    /* free memory */
    if (BCM2835 != NULL) { free(BCM2835); }

    return 0;
}


int bcm2835_spiTransfer(char *const data)
{
    volatile uint32_t *const fifoAddress = &(BCM2835->SPI0->FIFO);
    volatile char *const fifoCharacterAddress = (char*)fifoAddress;
    
    /* set transfer active */
    __sync_synchronize();
    BCM2835->SPI0->CS |= BCM2835_SPI0_CS_TA;
    __sync_synchronize();  /* TODO: replace extra __sync_synchronize with barrier() */

    while ((BCM2835->SPI0->CS & BCM2835_SPI0_CS_TXD) == 0) {}

    *fifoCharacterAddress = *data;

    /* wait until the transfer starts, then wait until it completes */
    while ((BCM2835->SPI0->CS & BCM2835_SPI0_CS_DONE) != 0) {}
    while ((BCM2835->SPI0->CS & BCM2835_SPI0_CS_DONE) == 0) {}

    /* copy read data into the data pointer */
    *data = *fifoCharacterAddress;

    /* unset transfer active */
    __sync_synchronize();
    BCM2835->SPI0->CS &= ~BCM2835_SPI0_CS_TA;
    __sync_synchronize();

    return 1;
}


/* Sets the pin mode of a GPIO pin.
 *    pinNumber - the number of the pin
 *    mode - the pin mode
 *    return - 1 if successful, 0 otherwise */
static int bcm2835_setPinMode(int pinNumber, PinMode mode)
{
    /* check for invalid pin mode */
    if (mode > NPinModes - 1)
    {
        return 0;
    }

    /* check for invalid pin number */
    if ((pinNumber < 0) || (pinNumber > NPins - 1))
    {
        return 0;
    }

    int registerOffset = pinNumber / NPinsPerGpfselRegister;
    int shiftAmount = GpfselModeWidth * (pinNumber % NPinsPerGpfselRegister);
    volatile uint32_t *gpfsel = &(BCM2835->GPIO->GPFSEL0) + registerOffset;
    
    const uint32_t writeValue = mode << shiftAmount;
    const uint32_t writeMask = GpioModeBaseMask << shiftAmount;

    bcm2835_setRegister(gpfsel, writeValue, writeMask);

    return 1;
}


/* Sets the bits of a register indicated by a 1 in "mask" to the corresponding value in "value".
 *    regAddr - address of register to modify
 *    value - new register values
 *    mask - indicates which bits of (*regAddr) to set
 *    return - 1 if successful, 0 otherwise */
static int bcm2835_setRegister(volatile uint32_t *const regAddr, const uint32_t value, const uint32_t mask)
{
    *regAddr = (*regAddr & ~mask) | value;

    return 1;
}
