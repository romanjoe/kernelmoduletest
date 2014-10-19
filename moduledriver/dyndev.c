#include <linux/cdev.h>
#include <linux/device.h>
#include "dev.h"

static int major = 0;
module_param( major, int, S_IRUGO );

#define EOK 0
static int device_open = 0;

static int dev_open( struct inode *n, struct file *f )
{
	if( device_open ) return -EBUSY;
	device_open++;

	return EOK; 
}

static int dev_release( struct inode *n, struct file *f )
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

#define DEVICE_FIRST	0
#define DEVICE_COUNT	3
#define MODNAME 		"my_dyndev_dev"

static struct cdev hcdev;
static struct class *devclass;

static int __init dev_init( void )
{
	int ret, i;
	dev_t dev;

	if( major != 0 ){
		dev = MKDEV( major, DEVICE_FIRST );
		ret = register_chrdev_region( dev, DEVICE_COUNT, MODNAME );
	} else{
		ret = alloc_chrdev_region( &dev, DEVICE_FIRST, DEVICE_COUNT, MODNAME );
		major = MAJOR( dev );
	}

	if( ret < 0 ){
		printk( KERN_ERR "=== Can not register char device region\n" );
		goto err;
	}

	cdev_init( &hcdev, &dev_fops );
	hcdev.owner = THIS_MODULE;
	ret = cdev_add( &hcdev, dev, DEVICE_COUNT );

	if ( ret < 0 ){
		unregister_chrdev_region( MKDEV( major, DEVICE_FIRST ), DEVICE_COUNT );
		printk( KERN_ERR "=== Can not add char device\n" );
		goto err;
	}

	devclass = class_create( THIS_MODULE, "dyn_class" );

#define DEVNAME "dyn"

	for (i = 0; i < DEVICE_COUNT; ++i)
	{
		char name[ 10 ];
		dev = MKDEV( major, DEVICE_FIRST + i );
		sprintf( name, "%s_%d", DEVNAME, i );
		device_create( devclass, NULL, dev, "%s", name ); 
	}

	printk( KERN_INFO "========= module instantiated %d:[%d-%d] ==========\n",
			(int) MAJOR( dev ), DEVICE_FIRST, (int) MINOR( dev ) );
err:
	return ret;
}

static void __exit dev_exit( void )
{
	dev_t dev;
	int i;

	for( i = 0; i < DEVICE_COUNT; i++ )
	{
		dev = MKDEV( major, DEVICE_FIRST + i);
		device_destroy( devclass, dev );
	}
	class_destroy( devclass );
	cdev_del( &hcdev );
	unregister_chrdev_region( MKDEV( major, DEVICE_FIRST ), DEVICE_COUNT );
	printk( KERN_INFO "========= module removed  ==========\n" );
}