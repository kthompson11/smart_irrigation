#ifndef BCM2835_SPI_H
#define BCM2835_SPI_H

/* Initializes the library. Must be called before any other library function. */
int bcm2835_spiInit(void);

/* Transfers and receives one byte of data.
 *    data - pointer to data to send and destination of received data */
int bcm2835_spiTransfer(char *data);

/* Frees library resources. No other library function should be called after bcm2835_spiFree. */
int bcm2835_spiFree(void);

#endif  /* BCM2835_SPI_H */