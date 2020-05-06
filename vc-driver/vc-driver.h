/* vc-driver.h - command interface to the valve-control driver */

#ifndef VC_DRIVER_H
#define VC_DRIVER_H

#include <linux/types.h>
#include <linux/ioctl.h>

#define VCBUF_SIZE 32


/****************************** write interface ******************************/
/* TODO: document write interface */

#define VCOPEN "open"
#define VCCLOSE "close"


/****************************** ioctl interface ******************************/
/* all reads and writes are passed by pointer */

/* driver magic number */
#define VC_IOC_MAGIC 0xEA

/* opens the indicated valve */
#define VC_IOC_WR_OPEN			_IOW(VC_IOC_MAGIC, 1, int)
/* closes the indicated valve */
#define VC_IOC_WR_CLOSE			_IOW(VC_IOC_MAGIC, 2, int)

/* reads the time duration of an open command (in milliseconds) */
#define VC_IOC_RD_OPENTIME		_IOR(VC_IOC_MAGIC, 1, int)
/* gets the number of valves */
#define VC_IOC_RD_NVALVES		_IOR(VC_IOC_MAGIC, 2, int)

#endif /* VC_DRIVER_H */