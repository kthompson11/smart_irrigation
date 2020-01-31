#ifndef BCM2835_CONSTANTS_H
#define BCM2835_CONSTANTS_H

#define N_PINS 54
#define N_PIN_MODES 8

#define REGISTER_WIDTH      4       /* register width in bytes */
#define PINS_PER_GPFSEL     10      /* number of pins configured per GPFSEL register */
#define GPFSEL_MASK         (uint32_t)0b111
#define GPFSEL_MASK_WIDTH   3

/* gpio pin modes (some modes may be incorrect, see bcm2835 arm peripheral errata) */
#define PIN_MODE_INPUT      0b000
#define PIN_MODE_OUTPUT     0b001
#define PIN_MODE_ALT0       0b100
#define PIN_MODE_ALT1       0b101
#define PIN_MODE_ALT2       0b110
#define PIN_MODE_ALT3       0b111
#define PIN_MODE_ALT4       0b011
#define PIN_MODE_ALT5       0b010

/* peripheral base physical addresses on the bcm2835 */
#define PERIPH_BASE 0x20000000
#define GPIO_BASE (PERIPH_BASE + 0x00200000)
#define SPI0_BASE (PERIPH_BASE + 0x00204000)

/* peripheral sizes */
#define GPIO_SIZE           0xa4
#define SPI0_SIZE           0x18

/* GPIO pheripheral register offsets */
#define GPIO_GPFSEL0        0x00
#define GPIO_GPFSEL1        0x04
#define GPIO_GPFSEL2        0x08
#define GPIO_GPFSEL3        0x0c
#define GPIO_GPFSEL4        0x10
#define GPIO_GPFSEL5        0x14
#define GPIO_GPSET0         0x1c
#define GPIO_GPSET1         0x20
#define GPIO_GPCLR0         0x28
#define GPIO_GPCLR1         0x2c
#define GPIO_GPLEV0         0x34
#define GPIO_GPLEV1         0x38
#define GPIO_GPEDS0         0x40
#define GPIO_GPEDS1         0x44
#define GPIO_GPREN0         0x4c
#define GPIO_GPREN1         0x50
#define GPIO_GPFEN0         0x58
#define GPIO_GPFEN1         0x5c
#define GPIO_GPHEN0         0x64
#define GPIO_GPHEN1         0x68
#define GPIO_GPLEN0         0x70
#define GPIO_GPLEN1         0x74
#define GPIO_GPAREN0        0x7c
#define GPIO_GPAREN1        0x80
#define GPIO_GPAFEN0        0x88
#define GPIO_GPAFEN1        0x8c
#define GPIO_GPPUD          0x94
#define GPIO_GPPUDCLK0      0x98
#define GPIO_GPPUDCLK1      0x9c

/* SPI0 peripheral register offsets */
#define SPI0_CS             0x00
#define SPI0_FIFO           0x04
#define SPI0_CLK            0x08
#define SPI0_DLEN           0x0c
#define SPI0_LTOH           0x10
#define SPI0_DC             0x14

/* SPI0 CS bits */
#define SPI0_CS_CS_SELECT0          ((uint32_t)0x00000000)
#define SPI0_CS_CS_SELECT1          ((uint32_t)0x00000001)
#define SPI0_CS_CS_SELECT2          ((uint32_t)0x00000002)
#define SPI0_CS_CPHA                ((uint32_t)0x00000004)
#define SPI0_CS_CPOL                ((uint32_t)0x00000008)
#define SPI0_CS_CLEAR_NOACTION      ((uint32_t)0x00000000)
#define SPI0_CS_CLEAR_CLEARTX       ((uint32_t)0x00000010)
#define SPI0_CS_CLEAR_CLEARRX       ((uint32_t)0x00000020)
#define SPI0_CS_CSPOL               ((uint32_t)0x00000040)
#define SPI0_CS_TA                  ((uint32_t)0x00000080)
#define SPI0_CS_DMAEN               ((uint32_t)0x00000100)
#define SPI0_CS_INTD                ((uint32_t)0x00000200)
#define SPI0_CS_INTR                ((uint32_t)0x00000400)
#define SPI0_CS_ADCS                ((uint32_t)0x00000800)
#define SPI0_CS_REN                 ((uint32_t)0x00001000)
#define SPI0_CS_LEN                 ((uint32_t)0x00002000)
#define SPI0_CS_LMONO               ((uint32_t)0x00004000)
#define SPI0_CS_TE_EN               ((uint32_t)0x00008000)
#define SPI0_CS_DONE                ((uint32_t)0x00010000)
#define SPI0_CS_RXD                 ((uint32_t)0x00020000)
#define SPI0_CS_TXD                 ((uint32_t)0x00040000)
#define SPI0_CS_RXR                 ((uint32_t)0x00080000)
#define SPI0_CS_RXF                 ((uint32_t)0x00100000)
#define SPI0_CS_CSPOL0              ((uint32_t)0x00200000)
#define SPI0_CS_CSPOL1              ((uint32_t)0x00400000)
#define SPI0_CS_CSPOL2              ((uint32_t)0x00800000)
#define SPI0_CS_DMA_LEN             ((uint32_t)0x01000000)
#define SPI0_CS_LEN_LONG            ((uint32_t)0x02000000)

#endif  /* BCM2835_CONSTANTS_H */
