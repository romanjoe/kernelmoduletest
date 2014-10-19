#include <linux/cdev.h>
#include "./dev.h"

static int major = 0;
module_param(major, int, S_IRUGO);

#define EOK 0
static int device_open = 0;

static int dev_open(struct inode * n, struct file * f)
{
	if(device_open)
		return -EBUSY;
	device_open++;
	return EOK;
}

static int dev_release(struct inode * n, struct file * f)
{
	device_open--;
	return EOK;
}

static const struct file_operations dev_fops = 
{
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read, 
};

#define DEVICE_FIRST 0
#define DEVICE_COUNT 4 // this is number of possible minor devices (0, 1, 2, 3) - 4 in total. 
					   // you can create @sudo mknod -m0666 /dev/my_dev4 c 258 4@, but can't use it al all
#define MODNAME "my_char_dev"

static struct cdev hcdev;

static int __init dev_init(void)
{
	int ret;
	dev_t dev;	// From 2.6 major and minor numbers are combined and stored in one 32 bit number of the data type dev_t.
				// Out of the 32 bits the MSB 12 bits represent the major number and the LSB 20 bits represent the minor number.

	if (major !=  0) // check if major number if not undefined - 0
	{
		dev = MKDEV(major, DEVICE_FIRST);
		ret = register_chrdev_region(dev, DEVICE_COUNT,MODNAME);
	} else
	{
		ret = alloc_chrdev_region(&dev, DEVICE_FIRST, DEVICE_COUNT, MODNAME);
		major = MAJOR(dev); // do not forget to fix!
	}
	if (ret < 0)
	{
		printk(KERN_INFO "=== Can not register device region\n");
		goto err;
	}
	cdev_init(&hcdev, &dev_fops);
	hcdev.owner = THIS_MODULE;
	ret = cdev_add(&hcdev, dev, DEVICE_COUNT);
	if(ret < 0)
	{
		unregister_chrdev_region(MKDEV(major, DEVICE_FIRST), DEVICE_COUNT);
		printk(KERN_ERR "=== Can not add char device\n");
		goto err;
	}
	printk(KERN_INFO "===== module is installed %d:%d=====",
			MAJOR(dev), MINOR(dev));
	err:
		return ret;
}
static void __exit dev_exit(void)
{
	cdev_del(&hcdev);
	unregister_chrdev_region(MKDEV(major, DEVICE_FIRST), DEVICE_COUNT);
	printk(KERN_INFO "===== module removed =====");
}
