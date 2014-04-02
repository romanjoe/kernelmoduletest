#include "poll.h"

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Roman Joe <mrromanjoe@gmail.com>" );
MODULE_VERSION( "5.2" );

static int pause = 100;
module_param( pause, int, S_IRUGO );

static struct private {				// device data block			
	atomic_t roff;					// offset for read
	char buf[ LEN_MSG + 2 ];		// data buffer
} devblock = {						// static initialization of what 
	.roff = ATOMIC_INIT( 0 ),		// will dynamically happen in open()
	.buf = "not initiazed yet!\n",
};

static struct private *dev = &devblock;

static DECLARE_WAIT_QUEUE_HEAD( qwait ); // wait queue

// read inside buffer (in devlock) until it ends
static ssize_t read( struct file *file, char *buf,
					 size_t count, loff_t *ppos )
{
	int len = 0;
	int off = atomic_read( &dev->roff );

	if( off > strlen( dev->buf ) )	// no available data
	{
		if( file->f_flags & O_NONBLOCK )
			return -EAGAIN;
		else interruptible_sleep_on( &qwait );
	}

	off = atomic_read( &dev->roff );	// read update

	if( off == strlen( dev->buf ) )
	{
		atomic_set( &dev->roff, off + 1 );
		return 0;
	}									// end of file EOF

	len = strlen( dev->buf ) - off;		// is data present?
	len = count < len ? count : len;

	if ( copy_to_user( buf, dev->buf + off, len ) )
		return -EFAULT;

	atomic_set( &dev->roff, off + len );
	return len;
}

// full update of buffer (and remove EOF)
static ssize_t write( struct file *file, const char *buf,
					size_t count, loff_t *ppos )
{
	int res, len = count < LEN_MSG ? count : LEN_MSG;
	res = copy_from_user( dev->buf, ( void* )buf, len );
	dev->buf[ len ] = '\0';				// reestablish the end of string
	
	if( '\n' != dev->buf[ len - 1 ] ) 
		strcat( dev->buf, "\n" );

	atomic_set( &dev->roff, 0 );		// enable next reading
	wake_up_interruptible( &qwait );

	return len;
}

// multiplexing poll/select (with dummy delay)
unsigned int poll( struct file *file, struct poll_table_struct *poll )
{
	int flag = POLLOUT | POLLWRNORM;
	poll_wait( file, &qwait, poll );
	sleep_on_timeout( &qwait, pause );

	if( atomic_read( &dev->roff ) <= strlen( dev->buf ) )
		flag |= ( POLLIN | POLLRDNORM );

	return flag;
}

// table of enabled operations for device
static const struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.read = read,
	.write = write,
	.poll = poll,	
};

// structure for device registration using misc_register() interface
static struct miscdevice pool_dev = 
{
	MISC_DYNAMIC_MINOR, DEVNAME, &fops
};

static int __init init( void )
{
	int ret = misc_register( &pool_dev ); 

	if( ret ) printk( KERN_ERR "unable to register device\n" ); 

	return ret; 
}

module_init( init ); 

static void __exit exit( void )
{
	misc_deregister( &pool_dev ); 
}

module_exit( exit );