#include "arg.c"

#define do_times3(arg1) \
__asm__ ( \
  "leal (%0,%0,2),%0 \n" \
  "movl %0,%%eax \n"  \
  : : "r" (arg1) : "%eax" );

int times3( int n ) {
   do_times3( n );
   return;
}

void do_fun( char* sarg ) {
   int n = atoi( sarg );
   printf( "%d -> ", n );
   n = times3( n );
   printf( "%d\n", n );
}

