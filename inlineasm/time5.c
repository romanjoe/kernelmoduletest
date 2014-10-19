#include "arg.c"

#define times5(arg1, arg2) \
__asm__ ( \
  "leal (%0,%0,4),%0" \
  : "=r" (arg2) \
  : "r" (arg1) );

void do_fun( char* sarg )
{
	int n = atoi( sarg );
	printf( "%d -> ", n );
	times5( n, n );
	printf( "%d\n", n );
}

