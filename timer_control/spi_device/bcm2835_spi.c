

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <asm/io.h>

#include "bcm2835_spi.h"
#include "bcm2835_constants.h"

/* TODO: move mappings to private_data */
static struct periph_mappings
{
    void __iomem *gpio;
    void __iomem *spi0;
} mappings;

static int is_gpio_reserved = 0;

static int set_pin_mode(int pin_number, uint32_t mode);
static void write_reg(void __iomem *base, uint32_t offset, uint32_t value);
static void write_reg_mask(void __iomem *base, uint32_t offset,
                           uint32_t value, uint32_t mask);
static uint32_t read_reg(void __iomem *base, uint32_t offset);

int bcm2835_spi0_init()
{
    /* TODO: free allocated resources on error (use local variables to keep track or use devm functions) */

    /* reserve peripheral address ranges */
    /* TODO: find better way to chose device name */
    if (!request_mem_region(GPIO_BASE, GPIO_SIZE, "spi_device"))
    {
        /* FIXME: prevent gpiolib from reserving this address. Do not use region unless it can be reserved */
        is_gpio_reserved = 0;
        printk(KERN_ERR KBUILD_MODNAME ": gpio address in use");
        /* return -EADDRINUSE; */
    }
    else
    {
        is_gpio_reserved = 1;
    }
    
    if (!request_mem_region(SPI0_BASE, SPI0_SIZE, "spi_device"))
    {
        printk(KERN_ERR KBUILD_MODNAME ": spi0 address in use");
        return -EADDRINUSE;
    }

    /* map peripherals into kernel virtual address space */
    mappings.gpio = ioremap_nocache(GPIO_BASE, GPIO_SIZE);
    if (!mappings.gpio)
    {
        printk(KERN_ERR KBUILD_MODNAME ": failed to remap GPIO memory");
        return -ENOMEM;  /* no space in virtual address space */
    }
    mappings.spi0 = ioremap_nocache(SPI0_BASE, SPI0_SIZE);
    if (!mappings.spi0)
    {
        printk(KERN_ERR KBUILD_MODNAME ": failed to remap SPI0 memory");
        return -ENOMEM;
    }
    
    /* configure GPIO pins for SPI0 */
    set_pin_mode(8, PIN_MODE_ALT0);
    set_pin_mode(9, PIN_MODE_ALT0);
    set_pin_mode(10, PIN_MODE_ALT0);
    set_pin_mode(11, PIN_MODE_ALT0);

    /* set SPI0 control register */
    uint32_t cs_value = (SPI0_CS_CS_SELECT0 | 
                         SPI0_CS_CLEAR_CLEARTX | 
                         SPI0_CS_CLEAR_CLEARRX);
    write_reg(mappings.spi0, SPI0_CS, cs_value);

    return 0;
}


int bcm2835_spi0_free(void)
{
    /* set gpio pins to default */
    set_pin_mode(8, PIN_MODE_INPUT);
    set_pin_mode(9, PIN_MODE_INPUT);
    set_pin_mode(10, PIN_MODE_INPUT);
    set_pin_mode(11, PIN_MODE_INPUT);

    /* unmap peripheral address mappings */
    if (mappings.gpio) { iounmap(mappings.gpio); }
    if (mappings.spi0) { iounmap(mappings.spi0); }

    /* release peripheral memory */
    /* TODO: gpio should always be reserved at this point (see fixme in init) */
    if (is_gpio_reserved) { 
        printk(KERN_INFO KBUILD_MODNAME ": releasing gpio mem");
        release_mem_region(GPIO_BASE, GPIO_SIZE);
    }
    release_mem_region(SPI0_BASE, SPI0_SIZE);

    return 0;
}

/* Transfers and recieves one byte of data over SPI0 */
void bcm2835_spi0_transfer(char *data)
{
    /* TODO: limit poll rate */
    /* TODO: set/clear bit function for setting single bits */
    /* set transfer active */
    write_reg_mask(mappings.spi0, SPI0_CS, SPI0_CS_TA, SPI0_CS_TA);

    /* wait for space in the TX FIFO */
    while ( !(read_reg(mappings.spi0, SPI0_CS) & SPI0_CS_TXD) ) {}

    write_reg(mappings.spi0, SPI0_FIFO, *data);

    /* wait until the transfer starts, then wait until it completes */
    while ( (read_reg(mappings.spi0, SPI0_CS) & SPI0_CS_DONE) ) {}
    while ( !(read_reg(mappings.spi0, SPI0_CS) & SPI0_CS_DONE) ) {}

    /* copy read data into the data pointer */
    *data = read_reg(mappings.spi0, SPI0_FIFO);

    /* unset transfer active */
    write_reg_mask(mappings.spi0, SPI0_CS, 0, SPI0_CS_TA);
}


/* Sets the pin mode of a GPIO pin. */
static int set_pin_mode(int pin_number, uint32_t mode)
{
    /* check for invalid pin mode */
    if (mode > N_PIN_MODES - 1)
    {
        return -EINVAL;
    }

    /* check for invalid pin number */
    if ((pin_number < 0) || (pin_number > N_PINS - 1))
    {
        return -EINVAL;
    }

    const int gpfsel_n = pin_number / PINS_PER_GPFSEL;
    const int gpfsel_offset = REGISTER_WIDTH * gpfsel_n;
    const int shift_amount = GPFSEL_MASK_WIDTH * (pin_number % PINS_PER_GPFSEL);
    const uint32_t write_value = mode << shift_amount;
    const uint32_t write_mask = GPFSEL_MASK << shift_amount;

    write_reg_mask(mappings.gpio, gpfsel_offset, write_value, write_mask);

    return 0;
}

/*
 * Writes a 32 bit unsigned integer to a register using masked bits.
 * Set bits (1) in the mask are written to the register. 
 * Unset bits (0) in the mask are unchanged.
 */
static void write_reg_mask(void __iomem *base, uint32_t offset,
                           uint32_t value, uint32_t mask)
{
    uint32_t reg_value = read_reg(base, offset);
    reg_value = (reg_value & ~mask) | (value & mask);
    write_reg(base, offset, reg_value);
}

/* Writes a 32 bit unsigned integer to a register. */
static void write_reg(void __iomem *base, uint32_t offset, uint32_t value)
{
    iowrite32(value, base + offset);
}

/* Reads a 32 bit unsigned integer from a register. */
static uint32_t read_reg(void __iomem *base, uint32_t offset)
{
    return ioread32(base + offset);
}
