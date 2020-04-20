#include <linux/module.h>
#include <linux/fs.h>
#include <linux/spi/spi.h>
#include <linux/cdev.h>

#define N_MINORS 1

/* device data */
struct device_data {
	dev_t devt;
	struct cdev cdev;
	spinlock_t lock;
	struct spi_device *spi;
	unsigned users;
};
/* For now, only one device is supported, so 
   store the device data as a global variable. */
static struct device_data *dev_data;

/* global driver data */
struct driver_data {
	dev_t devt;
	struct class *class;
};
static struct driver_data drv_data;

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
	int status;
	struct device_data *data;
	struct device *dev;

	/* check if a device is already handled by the driver */
	if (dev_data != NULL)
		return -ENODEV;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	/* initialize device data fields */
	data->devt = drv_data.devt; /* only one device */
	spin_lock_init(&data->lock);
	data->spi = spi;
	cdev_init(&data->cdev, &valve_control_fops);
	data->cdev.owner = THIS_MODULE;

	/* set spi deviver_data with the device's data */
	spi_set_drvdata(spi, data);

	/* store the device_data globally */
	dev_data = data;

	/* add device to system */
	status = cdev_add(&data->cdev, data->devt, 1);
	if (status)
		goto err_cdev_add;

	/* create device file */
	dev = device_create(drv_data.class, &spi->dev, data->devt, 
			    data, "valve-control");
	if (IS_ERR(dev)) {
		status = PTR_ERR(dev);
		goto err_device_create;
	}

	return 0;

err_device_create:
	cdev_del(&data->cdev);
err_cdev_add:
	kfree(data);
	return status;
}

static int valve_control_remove(struct spi_device *spi)
{
	struct device_data *data = spi_get_drvdata(spi);

	/* prevent further file operations */
	spin_lock_irq(&data->lock);
	data->spi = NULL;
	spin_unlock_irq(&data->lock);

	device_destroy(drv_data.class, data->devt);
	cdev_del(&data->cdev);
	kfree(data);
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

	drv_data.class = class_create(THIS_MODULE, "valve-control");
	if (IS_ERR(drv_data.class)) {
		status = PTR_ERR(drv_data.class);
		goto err_create_class;
	}

	status = spi_register_driver(&valve_control_driver);
	if (status < 0) {
		goto err_spi_register;
	}

	return status;

err_spi_register:
	class_destroy(drv_data.class);
err_create_class:
	unregister_chrdev_region(drv_data.devt, N_MINORS);
	return status;
}

module_init(valve_control_init);

static void __exit valve_control_exit(void)
{
	spi_unregister_driver(&valve_control_driver);
	class_destroy(drv_data.class);
	unregister_chrdev_region(drv_data.devt, N_MINORS);
}

module_exit(valve_control_exit);

MODULE_LICENSE("GPL");