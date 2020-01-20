#ifndef BCM2835_CONSTANTS_H
#define BCM2835_CONSTANTS_H

#include <stdint.h>

static const int NPinsPerGpfselRegister = 10;  /* number of pins handled by each GPFSEL register */
static const int GpfselModeWidth = 3;  /* width of the mode of each pin in a GPFSEL register */
static const uint32_t GpioModeBaseMask = 0b111; /* bitmask for pin modes */

static const int NPins = 54;  /* number of GPIO pins */
static const int NPinModes = 8;
static const char *const memDevicePath = "/dev/mem";

/* gpio pin modes (some modes may be incorrect, see bcm2835 arm peripheral errata) */
typedef uint32_t PinMode;
static const PinMode PIN_MODE_INPUT  = 0b000;
static const PinMode PIN_MODE_OUTPUT = 0b001;
static const PinMode PIN_MODE_ALT0   = 0b100;
static const PinMode PIN_MODE_ALT1   = 0b101;
static const PinMode PIN_MODE_ALT2   = 0b110;
static const PinMode PIN_MODE_ALT3   = 0b111;
static const PinMode PIN_MODE_ALT4   = 0b011;
static const PinMode PIN_MODE_ALT5   = 0b010;

/* peripheral base physical addresses on the bcm2835 */
#define BCM2835_PERIPH_PHYS_BASE 0x20000000
#define BCM2835_GPIO_BASE (BCM2835_PERIPH_PHYS_BASE + 0x00200000)
#define BCM2835_SPI0_BASE (BCM2835_PERIPH_PHYS_BASE + 0x00204000)

/* struct representing the GPIO peripheral */
typedef struct
{
    uint32_t GPFSEL0;     /* offset = 0x00 */
    uint32_t GPFSEL1;     /* offset = 0x04 */
    uint32_t GPFSEL2;     /* offset = 0x08 */
    uint32_t GPFSEL3;     /* offset = 0x0c */
    uint32_t GPFSEL4;     /* offset = 0x10 */
    uint32_t GPFSEL5;     /* offset = 0x14 */
    uint32_t RESERVED18;  /* offset = 0x18 */
    uint32_t GPSET0;      /* offset = 0x1c */
    uint32_t GPSET1;      /* offset = 0x20 */
    uint32_t RESERVED24;  /* offset = 0x24 */
    uint32_t GPCLR0;      /* offset = 0x28 */
    uint32_t GPCLR1;      /* offset = 0x2c */
    uint32_t RESERVED30;  /* offset = 0x30 */
    uint32_t GPLEV0;      /* offset = 0x34 */
    uint32_t GPLEV1;      /* offset = 0x38 */
    uint32_t RESERVED3C;  /* offset = 0x3c */
    uint32_t GPEDS0;      /* offset = 0x40 */
    uint32_t GPEDS1;      /* offset = 0x44 */
    uint32_t RESERVED48;  /* offset = 0x48 */
    uint32_t GPREN0;      /* offset = 0x4c */
    uint32_t GPREN1;      /* offset = 0x50 */
    uint32_t RESERVED54;  /* offset = 0x54 */
    uint32_t GPFEN0;      /* offset = 0x58 */
    uint32_t GPFEN1;      /* offset = 0x5c */
    uint32_t RESERVED60;  /* offset = 0x60 */
    uint32_t GPHEN0;      /* offset = 0x64 */
    uint32_t GPHEN1;      /* offset = 0x68 */
    uint32_t RESERVED6C;  /* offset = 0x6c */
    uint32_t GPLEN0;      /* offset = 0x70 */
    uint32_t GPLEN1;      /* offset = 0x74 */
    uint32_t RESERVED78;  /* offset = 0x78 */
    uint32_t GPAREN0;     /* offset = 0x7c */
    uint32_t GPAREN1;     /* offset = 0x80 */
    uint32_t RESERVED84;  /* offset = 0x84 */
    uint32_t GPAFEN0;     /* offset = 0x88 */
    uint32_t GPAFEN1;     /* offset = 0x8c */
    uint32_t RESERVED90;  /* offset = 0x90 */
    uint32_t GPPUD;       /* offset = 0x94 */
    uint32_t GPPUDCLK0;   /* offset = 0x98 */
    uint32_t GPPUDCLK1;   /* offset = 0x9c */
    uint32_t RESERVEDA0;  /* offset = 0xa0 */
} gpio_typedef;

/* struct representing the SPI0 peripheral */
typedef struct
{
    uint32_t CS;    /* offset = 0x00 */
    uint32_t FIFO;  /* offset = 0x04 */
    uint32_t CLK;   /* offset = 0x08 */
    uint32_t DLEN;  /* offset = 0x0c */
    uint32_t LTOH;  /* offset = 0x10 */
    uint32_t DC;    /* offset = 0x14 */
} spi0_typedef;

/* struct that holds the addresses of the peripherals */
typedef struct
{
    volatile gpio_typedef *GPIO;
    volatile spi0_typedef *SPI0;
} periph_mappings;

/* spio cs register bitmasks */
#define BCM2835_SPI0_CS_CS_SELECT0          ((uint32_t)0x00000000)
#define BCM2835_SPI0_CS_CS_SELECT1          ((uint32_t)0x00000001)
#define BCM2835_SPI0_CS_CS_SELECT2          ((uint32_t)0x00000002)
#define BCM2835_SPI0_CS_CPHA                ((uint32_t)0x00000004)
#define BCM2835_SPI0_CS_CPOL                ((uint32_t)0x00000008)
#define BCM2835_SPI0_CS_CLEAR_NOACTION      ((uint32_t)0x00000000)
#define BCM2835_SPI0_CS_CLEAR_CLEARTX       ((uint32_t)0x00000010)
#define BCM2835_SPI0_CS_CLEAR_CLEARRX       ((uint32_t)0x00000020)
#define BCM2835_SPI0_CS_CSPOL               ((uint32_t)0x00000040)
#define BCM2835_SPI0_CS_TA                  ((uint32_t)0x00000080)
#define BCM2835_SPI0_CS_DMAEN               ((uint32_t)0x00000100)
#define BCM2835_SPI0_CS_INTD                ((uint32_t)0x00000200)
#define BCM2835_SPI0_CS_INTR                ((uint32_t)0x00000400)
#define BCM2835_SPI0_CS_ADCS                ((uint32_t)0x00000800)
#define BCM2835_SPI0_CS_REN                 ((uint32_t)0x00001000)
#define BCM2835_SPI0_CS_LEN                 ((uint32_t)0x00002000)
#define BCM2835_SPI0_CS_LMONO               ((uint32_t)0x00004000)
#define BCM2835_SPI0_CS_TE_EN               ((uint32_t)0x00008000)
#define BCM2835_SPI0_CS_DONE                ((uint32_t)0x00010000)
#define BCM2835_SPI0_CS_RXD                 ((uint32_t)0x00020000)
#define BCM2835_SPI0_CS_TXD                 ((uint32_t)0x00040000)
#define BCM2835_SPI0_CS_RXR                 ((uint32_t)0x00080000)
#define BCM2835_SPI0_CS_RXF                 ((uint32_t)0x00100000)
#define BCM2835_SPI0_CS_CSPOL0              ((uint32_t)0x00200000)
#define BCM2835_SPI0_CS_CSPOL1              ((uint32_t)0x00400000)
#define BCM2835_SPI0_CS_CSPOL2              ((uint32_t)0x00800000)
#define BCM2835_SPI0_CS_DMA_LEN             ((uint32_t)0x01000000)
#define BCM2835_SPI0_CS_LEN_LONG            ((uint32_t)0x02000000)

#endif  /* BCM2835_CONSTANTS_H */
