#include <stdio.h>

int main( int argc, char ** argv )
{
	if( argc != 2 )
	{
		fprintf( stderr, "Error: need a symbol name\n" );
		return -1;
	}
	int c;
	int count = 0;
	printf( "unsigned char %s[] = {", argv[1] );
	while( ( c = getchar() ) != EOF )
	{
		if( ( count & 0x1f ) == 0 )
		{
			printf( "\n\t" );
		}
		printf( "0x%02x, ", c );
		count++;
	}
	printf( "};\n" );

}
