/* simple half-duplex spi character device */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#include "bcm2835_spi.h"

#define BUFFER_SIZE 2

/* TODO: move buffers to private data */
struct spi_dev_data
{
    char *txBuffer;
    char *rxBuffer;
};

static int IsDeviceOpen = 0;

static int open_spi_dev(struct inode *inode, struct file *file)
{
    /* only allow one connection at a time */
    if (IsDeviceOpen)
    {
        return -EBUSY;
    }

    IsDeviceOpen = 1;

    return 0;
}

static int release_spi_dev(struct inode *inode, struct file *file)
{
    IsDeviceOpen = 0;

    return 0;
}

/* writes "len" (maximum = KBUF_SIZE) bytes from "userBuffer" to the spi device
 * offset must be zero 
 * blocks until write is done */
static ssize_t write_spi_dev(struct file *file, const char __user *userBuffer, size_t len, loff_t *offset)
{
    if (*offset != 0)
    {
        printk(KERN_ERR KBUILD_MODNAME ": write() called with nonzero offset = %d\n", (int)*offset);
        return -EINVAL;
    }

    /* copy user data to kernel buffer */
    /* TODO: move buffers to file->private_data (allocating on stack is bad) */
    char txBuffer[BUFFER_SIZE];
    ssize_t txLen = BUFFER_SIZE < len ? BUFFER_SIZE : len;
    unsigned long res = copy_from_user(txBuffer, userBuffer, txLen);
    if (res != 0)  /* if there are uncopied bytes */
    {
        txLen -= res;
    }

    /* transfer bytes over spi */
    ssize_t txCount;
    for (txCount = 0; txCount < txLen; ++txCount)
    {
        char nextByte = txBuffer[txCount];
        bcm2835_spi0_transfer(&nextByte);
    }

    offset += txCount;

    return txCount;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = open_spi_dev,
    .release = release_spi_dev,
    .write = write_spi_dev
};

struct miscdevice spi_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "spi0",
    .fops = &fops
};

static int __init init_spi_dev(void)
{
    int res = misc_register(&spi_dev);
    if (res) 
    {
        printk(KERN_ERR KBUILD_MODNAME ": could not register module");
        goto out_err;
    }
    printk(KERN_INFO KBUILD_MODNAME ": device registered");

    /* initialize spi library */
    res = bcm2835_spi0_init();
    if (res) 
    {
        printk(KERN_INFO KBUILD_MODNAME ": failed to initialize SPI0 library");
        goto out_spi_err;
    }

    printk(KERN_INFO KBUILD_MODNAME ": module loaded\n");

    return 0;

/* cleanup on error */
out_spi_err:
    misc_deregister(&spi_dev);
out_err:
    return res;
}

static void __exit exit_spi_dev(void)
{
    /* free spi library resources */
    bcm2835_spi0_free();

   misc_deregister(&spi_dev);
   printk(KERN_INFO KBUILD_MODNAME ": module unloaded\n");
}

module_init(init_spi_dev);
module_exit(exit_spi_dev);
