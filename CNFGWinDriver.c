//Copyright (c) 2011-2019 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.
//Portion from: http://en.wikibooks.org/wiki/Windows_Programming/Window_Creation

#ifndef _CNFGWINDRIVER_C
#define _CNFGWINDRIVER_C

#include "CNFG.h"
#include <windows.h>
#include <stdlib.h>
#include <malloc.h> //for alloca
#include <ctype.h>

static HBITMAP lsBitmap;
static HWND lsHWND;
static HDC lsWindowHDC;
static HDC lsHDC;
static HDC lsHDCBlit;

//Queue up lines and points for a faster render.
#ifndef CNFG_WINDOWS_DISABLE_BATCH
#define BATCH_ELEMENTS
#endif

#define COLORSWAPS( RGB ) \
		((((RGB )& 0xFF000000)>>24) | ( ((RGB )& 0xFF0000 ) >> 8 ) | ( ((RGB )& 0xFF00 )<<8 ))


void CNFGChangeWindowTitle( const char * windowtitle )
{
	SetWindowTextA( lsHWND, windowtitle );
}

#ifdef CNFGRASTERIZER
#include "CNFGRasterizer.c"

void InternalHandleResize()
{
	if( lsBitmap ) DeleteObject( lsBitmap );

	CNFGInternalResize( bufferx, buffery );
	lsBitmap = CreateBitmap( bufferx, buffery, 1, 32, buffer );
	SelectObject( lsHDC, lsBitmap );
	CNFGInternalResize( bufferx, buffery);
}
#else
static short bufferx, buffery;
static void InternalHandleResize();
#endif


#ifdef CNFGOGL
#include <GL/gl.h>
static HGLRC           hRC=NULL; 
static void InternalHandleResize() { }
void CNFGSwapBuffers()
{
#ifdef CNFG_BATCH
	CNFGFlushRender();
#endif

	SwapBuffers(lsWindowHDC);
}
#endif

void CNFGGetDimensions( short * x, short * y )
{
	static short lastx, lasty;
	RECT window;
	GetClientRect( lsHWND, &window );
	bufferx = (short)( window.right - window.left);
	buffery = (short)( window.bottom - window.top);
	if( bufferx != lastx || buffery != lasty )
	{
		lastx = bufferx;
		lasty = buffery;
		CNFGInternalResize( lastx, lasty );
		InternalHandleResize();
	}
	*x = bufferx;
	*y = buffery;
}

#ifndef CNFGOGL
void CNFGUpdateScreenWithBitmap( uint32_t * data, int w, int h )
{
	RECT r;

	SelectObject( lsHDC, lsBitmap );
	SetBitmapBits(lsBitmap,w*h*4,data);
	BitBlt(lsWindowHDC, 0, 0, w, h, lsHDC, 0, 0, SRCCOPY);
	UpdateWindow( lsHWND );

	short thisw, thish;

	//Check to see if the window is closed.
	if( !IsWindow( lsHWND ) )
	{
		exit( 0 );
	}

	GetClientRect( lsHWND, &r );
	thisw = (short)(r.right - r.left);
	thish = (short)(r.bottom - r.top);
	if( thisw != bufferx || thish != buffery )
	{
		bufferx = thisw;
		buffery = thish;
		InternalHandleResize();
	}
}
#endif

void CNFGTearDown()
{
	PostQuitMessage(0);
#ifdef CNFGOGL
	exit(0);
#endif
}

//This was from the article
LRESULT CALLBACK MyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
#ifndef CNFGOGL
	case WM_SYSCOMMAND:  //Not sure why, if deactivated, the dc gets unassociated?
		if( wParam == SC_RESTORE || wParam == SC_MAXIMIZE || wParam == SC_SCREENSAVE )
		{
			SelectObject( lsHDC, lsBitmap );
			SelectObject( lsWindowHDC, lsBitmap );
		}
		break;
#endif
	case WM_DESTROY:
		HandleDestroy();
		CNFGTearDown();
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//This was from the article, too... well, mostly.
int CNFGSetup( const char * name_of_window, int width, int height )
{
	static LPSTR szClassName = "MyClass";
	RECT client, window;
	WNDCLASS wnd;
	int w, h, wd, hd;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	bufferx = (short)width;
	buffery = (short)height;

	wnd.style = CS_HREDRAW | CS_VREDRAW; //we will explain this later
	wnd.lpfnWndProc = MyWndProc;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hInstance = hInstance;
	wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION); //default icon
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);   //default arrow mouse cursor
	wnd.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wnd.lpszMenuName = NULL;                     //no menu
	wnd.lpszClassName = szClassName;

	if(!RegisterClass(&wnd))                     //register the WNDCLASS
	{
		MessageBox(NULL, "This Program Requires Windows NT", "Error", MB_OK);
	}

	lsHWND = CreateWindow(szClassName,
		name_of_window,      //name_of_window,
		WS_OVERLAPPEDWINDOW, //basic window style
		CW_USEDEFAULT,
		CW_USEDEFAULT,       //set starting point to default value
		bufferx,
		buffery,        //set all the dimensions to default value
		NULL,                //no parent window
		NULL,                //no menu
		hInstance,
		NULL);               //no parameters to pass

	lsWindowHDC = GetDC( lsHWND );

#ifdef CNFGOGL
	//From NeHe
	static  PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0, 
		0,
		0,
		0,
		0, 0, 0, 0,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	GLuint      PixelFormat = ChoosePixelFormat( lsWindowHDC, &pfd );
	if( !SetPixelFormat( lsWindowHDC, PixelFormat, &pfd ) )
	{
		MessageBox( 0, "Could not create PFD for OpenGL Context\n", 0, 0 );
		exit( -1 );
	}
	if (!(hRC=wglCreateContext(lsWindowHDC)))                   // Are We Able To Get A Rendering Context?
	{
		MessageBox( 0, "Could not create OpenGL Context\n", 0, 0 );
		exit( -1 );
	}
	if(!wglMakeCurrent(lsWindowHDC,hRC))                        // Try To Activate The Rendering Context
	{
		MessageBox( 0, "Could not current OpenGL Context\n", 0, 0 );
		exit( -1 );
	}
#endif

	lsHDC = CreateCompatibleDC( lsWindowHDC );
	lsHDCBlit = CreateCompatibleDC( lsWindowHDC );
	lsBitmap = CreateCompatibleBitmap( lsWindowHDC, bufferx, buffery );
	SelectObject( lsHDC, lsBitmap );

	//lsClearBrush = CreateSolidBrush( CNFGBGColor );
	//lsHBR = CreateSolidBrush( 0xFFFFFF );
	//lsHPEN = CreatePen( PS_SOLID, 0, 0xFFFFFF );

	ShowWindow(lsHWND, 1);              //display the window on the screen

	//Once set up... we have to change the window's borders so we get the client size right.
	GetClientRect( lsHWND, &client );
	GetWindowRect( lsHWND, &window );
	w = ( window.right - window.left);
	h = ( window.bottom - window.top);
	wd = w - client.right;
	hd = h - client.bottom;
	MoveWindow( lsHWND, window.left, window.top, bufferx + wd, buffery + hd, 1 );

	InternalHandleResize();

#ifdef CNFG_BATCH
	CNFGSetupBatchInternal();
#endif

	return 0;
}

void CNFGHandleInput()
{
	MSG msg;
	while( PeekMessage( &msg, lsHWND, 0, 0xFFFF, 1 ) )
	{
		TranslateMessage(&msg);

		switch( msg.message )
		{
		case WM_MOUSEMOVE:
			HandleMotion( (msg.lParam & 0xFFFF), (msg.lParam>>16) & 0xFFFF, ( (msg.wParam & 0x01)?1:0) | ((msg.wParam & 0x02)?2:0) | ((msg.wParam & 0x10)?4:0) );
			break;
		case WM_LBUTTONDOWN:	HandleButton( (msg.lParam & 0xFFFF), (msg.lParam>>16) & 0xFFFF, 1, 1 ); break;
		case WM_RBUTTONDOWN:	HandleButton( (msg.lParam & 0xFFFF), (msg.lParam>>16) & 0xFFFF, 2, 1 ); break;
		case WM_MBUTTONDOWN:	HandleButton( (msg.lParam & 0xFFFF), (msg.lParam>>16) & 0xFFFF, 3, 1 ); break;
		case WM_LBUTTONUP:		HandleButton( (msg.lParam & 0xFFFF), (msg.lParam>>16) & 0xFFFF, 1, 0 ); break;
		case WM_RBUTTONUP:		HandleButton( (msg.lParam & 0xFFFF), (msg.lParam>>16) & 0xFFFF, 2, 0 ); break;
		case WM_MBUTTONUP:		HandleButton( (msg.lParam & 0xFFFF), (msg.lParam>>16) & 0xFFFF, 3, 0 ); break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			HandleKey( tolower( (int) msg.wParam ), (msg.message==WM_KEYDOWN) );
			break;
		default:
			DispatchMessage(&msg);
			break;
		}
	}
}

#ifndef CNFGOGL

#ifndef CNFGRASTERIZER

static HBITMAP lsBackBitmap;
static HBRUSH lsHBR;
static HPEN lsHPEN;
static HBRUSH lsClearBrush;

static void InternalHandleResize()
{
	DeleteObject( lsBackBitmap );
	lsBackBitmap = CreateCompatibleBitmap( lsHDC, bufferx, buffery );
	SelectObject( lsHDC, lsBackBitmap );
}

#ifdef BATCH_ELEMENTS

static int linelisthead;
static int pointlisthead;
static int polylisthead;
static int polylistindex;
static POINT linelist[4096*3];
static DWORD twoarray[4096];
static POINT pointlist[4096];
static POINT polylist[8192];
static INT   polylistcutoffs[8192];


static int last_linex;
static int last_liney;
static int possible_lastline;

void FlushTacking()
{
	int i;

	if( twoarray[0] != 2 )
		for( i = 0; i < 4096; i++ ) twoarray[i] = 2;

	if( linelisthead )
	{
		PolyPolyline( lsHDC, linelist, twoarray, linelisthead );
		linelisthead = 0;
	}

	if( polylistindex )
	{
		PolyPolygon( lsHDC, polylist, polylistcutoffs, polylistindex );
		polylistindex = 0;
		polylisthead = 0;
	}

	if( possible_lastline )
		CNFGTackPixel( last_linex, last_liney );
	possible_lastline = 0;

	//XXX TODO: Consider locking the bitmap, and manually drawing the pixels.
	if( pointlisthead )
	{
		for( i = 0; i < pointlisthead; i++ )
		{
			SetPixel( lsHDC, pointlist[i].x, pointlist[i].y, CNFGLastColor );
		}
		pointlisthead = 0;
	}
}
#endif

uint32_t CNFGColor( uint32_t RGB )
{
	RGB = COLORSWAPS( RGB );
	if( CNFGLastColor == RGB ) return RGB;

#ifdef BATCH_ELEMENTS
	FlushTacking();
#endif

	CNFGLastColor = RGB;

	DeleteObject( lsHBR );
	lsHBR = CreateSolidBrush( RGB );
	SelectObject( lsHDC, lsHBR );

	DeleteObject( lsHPEN );
	lsHPEN = CreatePen( PS_SOLID, 0, RGB );
	SelectObject( lsHDC, lsHPEN );

	return RGB;
}

void CNFGBlitImage( uint32_t * data, int x, int y, int w, int h )
{
	static int pbw, pbh;
	static HBITMAP pbb;
	if( !pbb || pbw != w || pbh !=h )
	{
		if( pbb ) DeleteObject( pbb );
		pbb = CreateBitmap( w, h, 1, 32, 0 );
		pbh = h;
		pbw = w;
	}
	SetBitmapBits(pbb,w*h*4,data);
	SelectObject( lsHDCBlit, pbb );
	BitBlt(lsHDC, x, y, w, h, lsHDCBlit, 0, 0, SRCCOPY);
}

void CNFGTackSegment( short x1, short y1, short x2, short y2 )
{
#ifdef BATCH_ELEMENTS

	if( ( x1 != last_linex || y1 != last_liney ) && possible_lastline )
	{
		CNFGTackPixel( last_linex, last_liney );
	}

	if( x1 == x2 && y1 == y2 )
	{
		CNFGTackPixel( x1, y1 );
		possible_lastline = 0;
		return;
	}

	last_linex = x2;
	last_liney = y2;
	possible_lastline = 1;

	if( x1 != x2 || y1 != y2 )
	{
		linelist[linelisthead*2+0].x = x1;
		linelist[linelisthead*2+0].y = y1;
		linelist[linelisthead*2+1].x = x2;
		linelist[linelisthead*2+1].y = y2;
		linelisthead++;
		if( linelisthead >= 2048 ) FlushTacking();
	}
#else
	POINT pt[2] = { {x1, y1}, {x2, y2} };
	Polyline( lsHDC, pt, 2 );
	SetPixel( lsHDC, x1, y1, CNFGLastColor );
	SetPixel( lsHDC, x2, y2, CNFGLastColor );
#endif
}

void CNFGTackRectangle( short x1, short y1, short x2, short y2 )
{
#ifdef BATCH_ELEMENTS
	FlushTacking();
#endif
	RECT r;
	if( x1 < x2 ) { r.left = x1; r.right = x2; }
	else          { r.left = x2; r.right = x1; }
	if( y1 < y2 ) { r.top = y1; r.bottom = y2; }
	else          { r.top = y2; r.bottom = y1; }
	FillRect( lsHDC, &r, lsHBR );
}

void CNFGClearFrame()
{
#ifdef BATCH_ELEMENTS
	FlushTacking();
#endif
	RECT r = { 0, 0, bufferx, buffery };
	DeleteObject( lsClearBrush  );
	lsClearBrush = CreateSolidBrush( COLORSWAPS(CNFGBGColor) );
	SelectObject( lsHDC, lsClearBrush );
	FillRect( lsHDC, &r, lsClearBrush);
}

void CNFGTackPoly( RDPoint * points, int verts )
{
#ifdef BATCH_ELEMENTS
	if( verts > 8192 )
	{
		FlushTacking();
		//Fall-through
	}
	else
	{
		if( polylistindex >= 8191 || polylisthead + verts >= 8191 )
		{
			FlushTacking();
		}
		int i;
		for( i = 0; i < verts; i++ )
		{
			polylist[polylisthead].x = points[i].x;
			polylist[polylisthead].y = points[i].y;
			polylisthead++;
		}
		polylistcutoffs[polylistindex++] = verts;
		return;
	}
#endif
	{
		int i;
		POINT * t = (POINT*)alloca( sizeof( POINT ) * verts );
		for( i = 0; i < verts; i++ )
		{
			t[i].x = points[i].x;
			t[i].y = points[i].y;
		}
		Polygon( lsHDC, t, verts );
	}
}


void CNFGTackPixel( short x1, short y1 )
{
#ifdef BATCH_ELEMENTS
	pointlist[pointlisthead+0].x = x1;
	pointlist[pointlisthead+0].y = y1;
	pointlisthead++;

	if( pointlisthead >=4096 ) FlushTacking();
#else
	SetPixel( lsHDC, x1, y1, CNFGLastColor );
#endif

}

void CNFGSwapBuffers()
{
#ifdef BATCH_ELEMENTS
	FlushTacking();
#endif
	int thisw, thish;

	RECT r;
	BitBlt( lsWindowHDC, 0, 0, bufferx, buffery, lsHDC, 0, 0, SRCCOPY );
	UpdateWindow( lsHWND );
	//Check to see if the window is closed.
	if( !IsWindow( lsHWND ) )
	{
		exit( 0 );
	}

	GetClientRect( lsHWND, &r );
	thisw = r.right - r.left;
	thish = r.bottom - r.top;

	if( thisw != bufferx || thish != buffery )
	{
		bufferx = (short)thisw;
		buffery = (short)thish;
		InternalHandleResize();
	}
}

void CNFGInternalResize( short bfx, short bfy ) { }
#endif

#endif

#endif // _CNFGWINDRIVER_C

