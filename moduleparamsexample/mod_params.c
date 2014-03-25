#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/string.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR("Roman Joe <mrromanjoe@gmail.com>");

static int iparam = 0;
module_param ( iparam, int, 0 );

static int k = 0;
module_param_named( nparam, k, int, 0 );

static char* sparam;
module_param ( sparam, charp, 0 );

#define FIXLEN 5
static char s[ FIXLEN ] = "";
module_param_string( cparam, s, sizeof( s ), 0 );

static int aparam[] = { 0, 0, 0, 0, 0 };
static int arnum = sizeof( aparam ) / sizeof( aparam[ 0 ] );
module_param_array( aparam, int, &arnum, S_IRUGO | S_IWUSR );

static int __init mod_init( void )
{
	int j;
	char msg[ 40 ] = "";
	printk( "========================================\n" ); 
	printk( "iparam = %d\n", iparam ); 
   	printk( "nparam = %d\n", k ); 
   	printk( "sparam = %s\n", sparam ); 
   	printk( "cparam = %s {%d}\n", s, ( int )strlen( s ) ); 
   	sprintf( msg, "aparam [ %d ] = ", arnum ); 
   	
   	for( j = 0; j < arnum; j++ ) 
      sprintf( msg + strlen( msg ), " %d ", aparam[ j ] ); 
   	
   	printk( "%s\n========================================\n", msg ); 
   	
   	return -10000;
}

module_init( mod_init );