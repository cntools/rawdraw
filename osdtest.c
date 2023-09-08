//Copyright (c) 2011 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "CNFG.h"
#include "os_generic.h"

unsigned frames = 0;
unsigned long iframeno = 0;

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


int HandleDestroy()
{
	printf( "Destroying\n" );
	exit(10);
	return 0;
}


short screenx, screeny;

int main()
{
	int i, x, y;
	double ThisTime;
	double LastFPSTime = OGGetAbsoluteTime();
	double LastFrameTime = OGGetAbsoluteTime();
	double SecToWait;
	int linesegs = 0;

	CNFGBGColor = 0x800000;
	//Deprecated CNFGDialogColor = 0x444444;
	CNFGPrepareForTransparency();
	CNFGSetupFullscreen( "Test Bench", 0 );

	while(CNFGHandleInput())
	{
		int i, pos;
		float f;
		iframeno++;
		RDPoint pto[3];

		CNFGClearFrame();
		CNFGColor( 0xFFFFFF );
		CNFGGetDimensions( &screenx, &screeny );

//		DrawFrame();

/*

		pto[0].x = 100;
		pto[0].y = 100;
		pto[1].x = 200;
		pto[1].y = 100;
		pto[2].x = 100;
		pto[2].y = 200;
		CNFGTackPoly( &pto[0], 3 );

		CNFGColor( 0xFF00FF );
*/

/*		CNFGTackSegment( pto[0].x, pto[0].y, pto[1].x, pto[1].y );
		CNFGTackSegment( pto[1].x, pto[1].y, pto[2].x, pto[2].y );
		CNFGTackSegment( pto[2].x, pto[2].y, pto[0].x, pto[0].y );
*/

		CNFGClearTransparencyLevel();
		CNFGDrawToTransparencyMode(1);
		CNFGTackRectangle( 0, 0, 260, 260 );
		CNFGTackRectangle( 400, 400, 600, 600 );
		CNFGPenX = 300;
		CNFGPenY = 200;
		CNFGSetLineWidth( 7 + sin(iframeno)*5 );
		CNFGDrawText( "HELLO!", 5 );
		CNFGDrawToTransparencyMode(0);	


		CNFGSetLineWidth( 1 );
		CNFGDrawText( "HELLO!", 5 );

		CNFGDrawBox( 0, 0, 260, 260 );

		CNFGPenX = 10; CNFGPenY = 10;

		pos = 0;
		CNFGColor( 0xffffff );
		for( i = 0; i < 1; i++ )
		{
			int c;
			char tw[2] = { 0, 0 };
			for( c = 0; c < 256; c++ )
			{
				tw[0] = c;

				CNFGPenX = ( c % 16 ) * 16+5;
				CNFGPenY = ( c / 16 ) * 16+5;
				CNFGDrawText( tw, 2 );
			}
		}

		CNFGPenX = 0;
		CNFGPenY = 0;


		RDPoint pp[3];
		CNFGColor( 0x00FF00 );
		pp[0].x = (short)(-200*sin((float)(iframeno)*.01) + 500);
		pp[0].y = (short)(-200*cos((float)(iframeno)*.01) + 500);
		pp[1].x = (short)(200*sin((float)(iframeno)*.01) + 500);
		pp[1].y = (short)(00*cos((float)(iframeno)*.01) + 500);
		pp[2].x = (short)(200*sin((float)(iframeno)*.01) + 500);
		pp[2].y = (short)(200*cos((float)(iframeno)*.01) + 500);
		CNFGTackPoly( pp, 3 );


		frames++;
		CNFGSwapBuffers();

		ThisTime = OGGetAbsoluteTime();
		if( ThisTime > LastFPSTime + 1 )
		{
			printf( "FPS: %d\n", frames );
			frames = 0;
			linesegs = 0;
			LastFPSTime+=1;
		}

		SecToWait = .016 - ( ThisTime - LastFrameTime );
		LastFrameTime += .016;
		if( SecToWait > 0 )
			OGUSleep( (int)( SecToWait * 1000000 ) );
	}

	return(0);
}

