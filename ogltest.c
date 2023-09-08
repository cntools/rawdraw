#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

#include "CNFG.h"

void HandleKey( int keycode, int bDown )
{
	printf( "Key: %d %d\n", keycode, bDown );
}

void HandleButton( int x, int y, int button, int bDown )
{
}

void HandleMotion( int x, int y, int mask )
{
}

int HandleDestroy()
{
	exit(10);
	return 0;
}

int main()
{
	short w, h;
	int frameno = 0;
	CNFGSetup( "Test", 100, 100 );

	while(CNFGHandleInput())
	{
		CNFGClearFrame();

		glRotatef( frameno/100.0, 0, 0, 1);

		glColor4f( 0, 0, .5, 1 );

		glBegin( GL_TRIANGLES );
		glVertex3f( -40, -40, 0 );
		glVertex3f( 40, 0, 0 );
		glVertex3f( 0, 40, 0 );
		glEnd();

		char stmp[100];
		CNFGColor( 0xFFFFFF );
		sprintf( stmp, "%d\n", frameno );
		CNFGPenX = 10; CNFGPenY = 10;
		CNFGDrawText( stmp, 3 );
		CNFGTackPixel( 100, 100 );

		CNFGSwapBuffers();
		frameno++;
	}
}
