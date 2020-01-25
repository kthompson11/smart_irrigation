/* simple half-duplex spi character device */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>

static const struct file_operations fops = {
    .owner = THIS_MODULE
};

struct miscdevice spi_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "spi_dev",
    .fops = &fops
};

static int __init init_spi_dev(void)
{
    printk(KERN_INFO KBUILD_MODNAME ": module loaded\n");

    return 0;
}

static void __exit exit_spi_dev(void)
{
    printk(KERN_INFO KBUILD_MODNAME ": module unloaded\n");

    return;
}

module_init(init_spi_dev);
module_exit(exit_spi_dev);
