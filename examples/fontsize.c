#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../CNFG.h"
#include "../os_generic.h"
#include "../CNFG3D.h"

#if defined( CNFGOGL )
#define TITLESTRING "Fontsize test with OpenGL"
#elif defined( CNFGRASTERIZER )
#define TITLESTRING "Fontsize test with Rasterizer"
#else
#define TITLESTRING "Fontsize test with Unknown"
#endif

void HandleKey( int keycode, int bDown )
{
	if( keycode == 65307 ) exit( 0 );
	printf( "Key: %d -> %d\n", keycode, bDown );
}

void HandleButton( int x, int y, int button, int bDown )
{
	printf( "Button: %d,%d (%d) -> %d\n", x, y, button, bDown );
}

void HandleMotion( int x, int y, int mask )
{
//	printf( "Motion: %d,%d (%d)\n", x, y, mask );
}

void HandleDestroy()
{
	printf( "Destroying\n" );
	exit(10);
}

int main()
{

	int i, x, y;
	double ThisTime;
	double LastFPSTime = OGGetAbsoluteTime();
	double LastFrameTime = OGGetAbsoluteTime();
	double SecToWait;

	CNFGBGColor = 0x800000;
	CNFGDialogColor = 0x444444;
	CNFGSetup( TITLESTRING, 1220, 480 );



	while ( 1 ) {
		CNFGHandleInput();
		CNFGClearFrame();
		CNFGColor( 0xffffff );
		for( i = 1; i < 5; i++ )
		{
			int c;
			char tw[2] = { 0, 0 };
			for( c = 0; c < 256; c++ )
			{
				tw[0] = c;

				CNFGPenX = ( c % 16 ) * 16 + 5 + (i - 1) * 320;
				CNFGPenY = ( c / 16 ) * 18 + 5;
				CNFGDrawText( tw, i );
			}
		}
		
		CNFGPenX = 20;
		CNFGPenY = 300;
		for( i = 1; i < 7; i++) {
			CNFGPenY += i * 6;
			CNFGDrawText("A quick brown fox jumps over the lazy dog.", i);
		}
		CNFGSwapBuffers();
		OGUSleep( (int)( 0.5 * 1000000 ) );
	}
}
