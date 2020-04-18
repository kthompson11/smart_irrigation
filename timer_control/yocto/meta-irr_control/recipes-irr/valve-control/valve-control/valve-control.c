#include <linux/module.h>
#include <linux/fs.h>
#include <linux/spi/spi.h>

/************************* file_operations functions **************************/

static ssize_t valve_control_read(struct file *filp, char __user *buf, size_t size, loff_t *pos)
{
	return 0;
}

static ssize_t valve_control_write(struct file *filp, const char __user *buf, size_t size, loff_t *pos)
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
	{ .compatible = "my_drivers_kt,valve_control", },
	{},
};

static struct spi_driver valve_control_driver = {
	.driver = {
		.name = "valve_control",
		.of_match_table = valve_control_of_match,
	},
	.probe = valve_control_probe,
	.remove = valve_control_remove,
};

/************************* module init/exit functions *************************/

static int __init valve_control_init(void)
{
	return 0;
}

module_init(valve_control_init);

static void valve_control_exit(void)
{
}

module_exit(valve_control_exit);

MODULE_LICENSE("MIT");