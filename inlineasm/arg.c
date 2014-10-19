#include <stdio.h>
#include <stdlib.h>

void do_fun( char* );

int main( int argc, char *argv[] )
{
	while( 1 ) {

		char buf[ 80 ];
	
		printf( "> " );
		fflush( stdout );
		gets( buf );
		do_fun( buf );
   }
   
   return 0;
};
