//Make it so we don't need to include any other C files in our build.
#define CNFG_IMPLEMENTATION

//Optional: Use OpenGL to do rendering on appropriate systems.
#define CNFGOGL

#include "CNFG.h"

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }
int main()
{
	CNFGSetup( "Example App", 1024, 768 );
	while(1)
	{
		CNFGBGColor = 0x000080ff; //Dark Blue Background

		short w, h;
		CNFGClearFrame();
		CNFGHandleInput();
		CNFGGetDimensions( &w, &h );

		//Change color to white.
		CNFGColor( 0xffffffff ); 

		CNFGPenX = 1; CNFGPenY = 1;
		CNFGDrawText( "Hello, World", 2 );
		//Draw a white pixel at 3,0 30 
		CNFGTackPixel( 30, 30 );         

		//Draw a line from 50,50 to 100,50
		CNFGTackSegment( 50, 50, 100, 50 );

		//Dark Red Color Select
		CNFGColor( 0x800000FF ); 

		//Draw 50x50 box starting at 100,50
		CNFGTackRectangle( 100, 50, 150, 100 ); 

		//Bright Purple Select
		CNFGColor( 0x800000FF ); 

		//Draw a triangle
		RDPoint points[3] = { { 30, 36}, {20, 50}, { 40, 50 } };
		CNFGTackPoly( points, 3 );

		//Draw a bunch of random pixels as a blitted image.
		{
			static uint32_t data[64*64];
			int x, y;

			for( y = 0; y < 64; y++ ) for( x = 0; x < 64; x++ )
				data[x+y*64] = 0xff | (rand()<<8);

			CNFGBlitImage( data, 150, 30, 64, 64 );
		}

		//Display the image and wait for time to display next frame.
		CNFGSwapBuffers();		
	}
}
