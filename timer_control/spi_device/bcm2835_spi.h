#ifndef BCM2835_SPI_H
#define BCM2835_SPI_H

/* Initializes the library. Must be called before any other library function. */
int bcm2835_spi0_init(void);

/* Transfers and receives one byte of data.
 *    data - pointer to data to send and destination of received data */
void bcm2835_spi0_transfer(char *data);

/* Frees library resources. No other library function should be called after bcm2835_spiFree. */
int bcm2835_spi0_free(void);

#endif  /* BCM2835_SPI_H */