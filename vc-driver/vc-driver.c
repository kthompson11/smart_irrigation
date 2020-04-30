#include <linux/module.h>
#include <linux/fs.h>
#include <linux/spi/spi.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "vc-driver.h"
#include "vc-interface.h"

#define N_MINORS 1

/* device data */
struct device_data {
	dev_t devt;
	struct cdev cdev;
	spinlock_t lock;
	struct spi_device *spi;
	unsigned users;
	char *buf;
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

static int op_write(u8 cmd, u8 arg, struct spi_device *spi)
{
	u8 request = vc_make_request(cmd, arg);
	struct spi_transfer tx;
	struct spi_message message;
	/* abort and return error code if request is invalid */
	if (!vc_is_request_valid(request)) {
		return -EFAULT; /* TODO: determine correct error code */
	}
	tx.tx_buf = &request;
	tx.len = 1;

	spi_message_init(&message);
	spi_message_add_tail(&tx, &message);
	
	return spi_sync(spi, &message);
}

/************************* file_operations functions **************************/

static ssize_t valve_control_read(struct file *filp, char __user *buf, 
		size_t size, loff_t *pos)
{
	return size;
}

static ssize_t valve_control_write(struct file *filp, const char __user *buf, 
		size_t size, loff_t *pos)
{
	/* TODO: more robust checking of input */
	struct device_data *data = filp->private_data;
	char *cmd, *next_token;
	unsigned int arg;

	if (size > VCBUF_SIZE)
		return -EMSGSIZE;

	data->buf[size] = 0;
	if (copy_from_user(data->buf, buf, size))
		return -EFAULT;

	/* get command token */
	next_token = data->buf;
	cmd = strsep(&next_token, " ");
	
	/* get the argument */
	if (kstrtouint(next_token, 10, &arg))
		return -EFAULT; /* TODO: determine correct error code */

	/* decode command token */
	/* TODO: check arg with function from valve_messaging.h */
	spin_lock_irq(&data->lock);
	if (!strcmp(cmd, VCOPEN))
		op_write(VC_OPCODE_OPEN, arg, data->spi);
	else if (!strcmp(cmd, VCCLOSE))
		op_write(VC_OPCODE_CLOSE, arg, data->spi);
	else
		return -EFAULT; /* TODO: determine corect error code */
	spin_unlock_irq(&data->lock);

	return size;
}

static int valve_control_open(struct inode *inode, struct file *filp)
{
	struct device_data *data = container_of(inode->i_cdev, 
						struct device_data, cdev);

	/* limit to one open at a time */
	spin_lock_irq(&data->lock);
	if (data->users != 0)
		return -EBUSY;
	else
		data->users += 1;
	spin_unlock_irq(&data->lock);

	filp->private_data = data;
	return 0;
}

static int valve_control_release(struct inode *inode, struct file *filp)
{
	struct device_data *data = filp->private_data;

	spin_lock_irq(&data->lock);
	data->users -= 1;
	spin_unlock_irq(&data->lock);

	/* TODO: free data if device no longer exists */

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
	data->buf = kzalloc(VCBUF_SIZE, GFP_KERNEL);

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
	kfree(data->buf);
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
	kfree(data->buf);
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
