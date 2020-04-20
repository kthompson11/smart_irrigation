#include <linux/module.h>
#include <linux/fs.h>
#include <linux/spi/spi.h>

#define N_MINORS 1

struct valve_control_data {
	dev_t devt;
	spinlock_t lock;
	struct spi_device *spi;
	unsigned users;
};
/* for now, put all data in one struct because only one device is supported */
static struct valve_control_data drv_data;

static struct class *valve_control_class;

/************************* file_operations functions **************************/

static ssize_t valve_control_read(struct file *filp, char __user *buf, 
		size_t size, loff_t *pos)
{
	return 0;
}

static ssize_t valve_control_write(struct file *filp, const char __user *buf, 
		size_t size, loff_t *pos)
{
	return 0;
}

static int valve_control_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int valve_control_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations valve_control_fops = {
	.owner = THIS_MODULE,
	.llseek = no_llseek,
	.read = valve_control_read,
	.write = valve_control_write,
	.open = valve_control_open,
	.release = valve_control_release,
};

/**************************** spi_driver functions ****************************/

static int valve_control_probe(struct spi_device *spi)
{
	return 0;
}

static int valve_control_remove(struct spi_device *spi)
{
	return 0;
}

static const struct of_device_id valve_control_of_match[] = {
	{ .compatible = "my_devices_kt,valve-control", },
	{},
};

static struct spi_driver valve_control_driver = {
	.driver = {
		.name = "valve-control",
		.of_match_table = valve_control_of_match,
	},
	.probe = valve_control_probe,
	.remove = valve_control_remove,
};

/************************* module init/exit functions *************************/

static int __init valve_control_init(void)
{
	int status;

	status = alloc_chrdev_region(&drv_data.devt, 0, N_MINORS, "valve-control");
	if (status < 0) {
		return status;
	}

	valve_control_class = class_create(THIS_MODULE, "valve-control");
	if (IS_ERR(valve_control_class)) {
		status = PTR_ERR(valve_control_class);
		goto err_create_class;
	}

	status = spi_register_driver(&valve_control_driver);
	if (status < 0) {
		goto err_spi_register;
	}

	return status;

err_spi_register:
	class_destroy(valve_control_class);
err_create_class:
	unregister_chrdev_region(drv_data.devt, N_MINORS);
	return status;
}

module_init(valve_control_init);

static void __exit valve_control_exit(void)
{
	spi_unregister_driver(&valve_control_driver);
	class_destroy(valve_control_class);
	unregister_chrdev_region(drv_data.devt, N_MINORS);
}

module_exit(valve_control_exit);

MODULE_LICENSE("GPL");