#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * hitfiles;

char * OSGLineFromFile( FILE * f )
{
	int c;
	int mlen = 128;
	int len = 0;

	if( !f || ferror(f) || feof( f ) ) return 0;

	char * ret = malloc( mlen );

	while( 1 )
	{
		c = fgetc( f );
		if( c == EOF || c == 0 || c == '\n' ) break;
		if( c == '\r' ) continue;
		
		ret[len++] = c;
		if( len >= mlen-1 )
		{
			mlen += 128;
			ret = realloc( ret, mlen );
		}
	}
	ret[len] = 0;
	return ret;
}

void IncludeFile( char * filename, char ** includes, int nrinc )
{
	FILE * f = fopen( filename, "rb" );
	if( !f )
	{
		fprintf( stderr, "Error: Could not open %s\n", filename );
		exit( -5 );
	}
	while( !feof( f ) )
	{
		char * line = OSGLineFromFile( f );
		if( line )
		{
			int match = 0;
			if( strncmp( line, "#include \"", 10 ) == 0 )
			{
				int i;
				for( i = 0; i < nrinc; i++ )
				{
					if( strncmp( line + 10, includes[i], strlen( includes[i] ) ) == 0 )
					{
						if( !hitfiles[i] )
						{
							IncludeFile( includes[i], includes, nrinc );
							hitfiles[i] = 0;
						}
						match = 1;
					}
				}
			}
			if( !match )
			{
				puts( line );
			}
		}
	}
}

int main( int argc, char ** argv )
{
	if( argc < 3 )
	{
		fprintf( stderr, "Error: Usage: single_file_creator {file to turn into a single file} {list of files to include}\n" );
		return -5;
	}

	hitfiles = malloc( argc );
	memset( hitfiles, 0, sizeof(hitfiles) );
	hitfiles[0] = 1;
	IncludeFile( argv[1], argv + 1, argc - 1 );

	return 0;
}
