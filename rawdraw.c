//Copyright (c) 2011 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "os_generic.h"

#define CNFG3D
#define CNFG_IMPLEMENTATION
//#define CNFGOGL
//#define CNFGRASTERIZER
//#define CNFG_WINDOWS_DISABLE_BATCH

#include "CNFG.h"

#define gx 6
#define gy 6
int margin = 5;
float gsx = 50;
float gsy = 50;
float ofsx, ofsy;

int grid[gx][gy];

void InvCell( int cx, int cy )
{
	if( cx >= 0 && cx < gx && cy >= 0 && cy < gy )
	{
		grid[cx][cy] ^= 1;
	}
}

void InvCross( int cx, int cy )
{
	InvCell( cx, cy );
	InvCell( cx+1, cy );
	InvCell( cx-1, cy );
	InvCell( cx, cy+1 );
	InvCell( cx, cy-1 );
}

void HandleKey( int keycode, int bDown )
{
}

void HandleButton( int x, int y, int button, int bDown )
{
	if( !bDown ) return;

	int tsx = gsx+margin;
	int tsy = gsy+margin;

	x -= ofsx;
	y -= ofsy;
	if( x%tsx > gsx ) return;
	if( y%tsy > gsy ) return;
	int coordx = x / (gsx+margin);
	int coordy = y / (gsy+margin);
	if( coordx < gx && coordy < gy )
	{
		InvCross( coordx, coordy );
	}
}

void HandleMotion( int x, int y, int mask )
{
}

int HandleDestroy()
{
	printf( "Destroying\n" );
	exit(10);
	return 0;
}

int main()
{

	CNFGBGColor = 0x000080FF; //Darkblue
	CNFGSetup( "Test Bench", 640, 480 );

	srand( OGGetAbsoluteTime() );

	int i;
	for( i = 0; i < 5; i++ )
	{
		InvCross( rand()%gx, rand()%gy );
	}

	while(CNFGHandleInput())
	{
		short wx, wy;
		CNFGGetDimensions( &wx, &wy );

		float scalex = wx / gx;
		float scaley = wy / gy;
		float scale = scaley;
		if( scalex < scaley ) scale = scalex;
		gsx = gsy = scale-margin;

		float extraw = wx - gsx * gx;
		float extrah = wy - gsy * gy;
		ofsx = extraw/2;
		ofsy = extrah/2;

		CNFGClearFrame();
		int x;
		int y;

		for( y = 0; y < gy; y++ )
		for( x = 0; x < gx; x++ )
		{
			int xco = x * (gsy+margin) + ofsx;
			int yco = y * (gsx+margin) + ofsy;
			CNFGColor( grid[x][y]?0xffffffff:0x00000ff );
			CNFGTackRectangle( xco+margin/2, yco+margin/2, xco+gsx, yco+gsy );
		}

		CNFGSwapBuffers();
	}}

