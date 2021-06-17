//Copyright (c) 2011, 2017, 2018 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.
//portions from 
//http://www.xmission.com/~georgeps/documentation/tutorials/Xlib_Beginner.html

//#define HAS_XINERAMA
//#define CNFG_HAS_XSHAPE
//#define FULL_SCREEN_STEAL_FOCUS

#ifndef _CNFGXDRIVER_C
#define _CNFGXDRIVER_C

#include "CNFG.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef HAS_XINERAMA
	#include <X11/extensions/shape.h>
	#include <X11/extensions/Xinerama.h>
#endif
#ifdef CNFG_HAS_XSHAPE
	#include <X11/extensions/shape.h>
	static    XGCValues xsval;
	static    Pixmap xspixmap;
	static    GC xsgc;

	static	int taint_shape;
	static	int prepare_xshape;
	static int was_transp;

#endif

#ifdef CNFG_BATCH
void CNFGSetupBatchInternal();
#endif

XWindowAttributes CNFGWinAtt;
XClassHint *CNFGClassHint;
char *wm_res_name = "rawdraw";
char *wm_res_class = "rawdraw";
Display *CNFGDisplay;
Window CNFGWindow;
int CNFGWindowInvisible;
Pixmap CNFGPixmap;
GC     CNFGGC;
GC     CNFGWindowGC;
Visual * CNFGVisual;
int CNFGX11ForceNoDecoration;
XImage *xi;

int g_x_global_key_state;
int g_x_global_shift_key;

void 	CNFGSetWindowIconData( int w, int h, uint32_t * data )
{
	static Atom net_wm_icon;
	static Atom cardinal; 

	if( !net_wm_icon ) net_wm_icon = XInternAtom( CNFGDisplay, "_NET_WM_ICON", False );
	if( !cardinal ) cardinal = XInternAtom( CNFGDisplay, "CARDINAL", False );

	unsigned long outdata[w*h];
	int i;
	for( i = 0; i < w*h; i++ )
	{
		outdata[i+2] = data[i];
	}
	outdata[0] = w;
	outdata[1] = h;
	XChangeProperty(CNFGDisplay, CNFGWindow, net_wm_icon, cardinal,
		32, PropModeReplace, (const unsigned char*)outdata, 2 + w*h);
}


#ifdef CNFG_HAS_XSHAPE
void	CNFGPrepareForTransparency() { prepare_xshape = 1; }
void	CNFGDrawToTransparencyMode( int transp )
{
	static Pixmap BackupCNFGPixmap;
	static GC     BackupCNFGGC;
	if( was_transp && ! transp )
	{
		CNFGGC = BackupCNFGGC;
		CNFGPixmap = BackupCNFGPixmap;
	}
	if( !was_transp && transp )
	{
		BackupCNFGPixmap = CNFGPixmap;
		BackupCNFGGC = CNFGGC;
		taint_shape = 1;
		CNFGGC = xsgc;
		CNFGPixmap = xspixmap;
	}
	was_transp = transp;
}
void	CNFGClearTransparencyLevel()
{
	taint_shape = 1;
	XSetForeground(CNFGDisplay, xsgc, 0);
	XFillRectangle(CNFGDisplay, xspixmap, xsgc, 0, 0, CNFGWinAtt.width, CNFGWinAtt.height);
	XSetForeground(CNFGDisplay, xsgc, 1);
}
#endif

#ifdef CNFGOGL
#include <GL/glx.h>
#include <GL/glxext.h>

GLXContext CNFGCtx;
void * CNFGGetExtension( const char * extname ) { return (void*)glXGetProcAddressARB((const GLubyte *) extname); }
#endif

int FullScreen = 0;

void CNFGGetDimensions( short * x, short * y )
{
	static int lastx;
	static int lasty;

	*x = CNFGWinAtt.width;
	*y = CNFGWinAtt.height;

	if( lastx != *x || lasty != *y )
	{
		lastx = *x;
		lasty = *y;
		CNFGInternalResize( lastx, lasty );
	}
}

void	CNFGChangeWindowTitle( const char * WindowName )
{
	XSetStandardProperties( CNFGDisplay, CNFGWindow, WindowName, 0, 0, 0, 0, 0 );
}

static void InternalLinkScreenAndGo( const char * WindowName )
{
	XFlush(CNFGDisplay);
	XGetWindowAttributes( CNFGDisplay, CNFGWindow, &CNFGWinAtt );

	XGetClassHint( CNFGDisplay, CNFGWindow, CNFGClassHint );
	if (!CNFGClassHint) {
		CNFGClassHint = XAllocClassHint();
		if (CNFGClassHint) {
			CNFGClassHint->res_name = wm_res_name;
			CNFGClassHint->res_class = wm_res_class;
			XSetClassHint( CNFGDisplay, CNFGWindow, CNFGClassHint );
		} else {
			fprintf( stderr, "Failed to allocate XClassHint!\n" );
		}
	} else {
		fprintf( stderr, "Pre-existing XClassHint\n" );
	}

	XSelectInput (CNFGDisplay, CNFGWindow, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | PointerMotionMask );


	CNFGWindowGC = XCreateGC(CNFGDisplay, CNFGWindow, 0, 0);


	if( CNFGX11ForceNoDecoration )
	{
		Atom window_type = XInternAtom(CNFGDisplay, "_NET_WM_WINDOW_TYPE", False);
		long value = XInternAtom(CNFGDisplay, "_NET_WM_WINDOW_TYPE_SPLASH", False);
		XChangeProperty(CNFGDisplay, CNFGWindow, window_type,
		   XA_ATOM, 32, PropModeReplace, (unsigned char *) &value,1 );
	}

	CNFGPixmap = XCreatePixmap( CNFGDisplay, CNFGWindow, CNFGWinAtt.width, CNFGWinAtt.height, CNFGWinAtt.depth );
	CNFGGC = XCreateGC(CNFGDisplay, CNFGPixmap, 0, 0);
	XSetLineAttributes(CNFGDisplay, CNFGGC, 1, LineSolid, CapRound, JoinRound);
	CNFGChangeWindowTitle( WindowName );
	if( !CNFGWindowInvisible )
		XMapWindow(CNFGDisplay, CNFGWindow);

#ifdef CNFG_HAS_XSHAPE
	if( prepare_xshape )
	{
	    xsval.foreground = 1;
	    xsval.line_width = 1;
	    xsval.line_style = LineSolid;
	    xspixmap = XCreatePixmap(CNFGDisplay, CNFGWindow, CNFGWinAtt.width, CNFGWinAtt.height, 1);
	    xsgc = XCreateGC(CNFGDisplay, xspixmap, 0, &xsval);
		XSetLineAttributes(CNFGDisplay, xsgc, 1, LineSolid, CapRound, JoinRound);
	}
#endif
}

void CNFGSetupFullscreen( const char * WindowName, int screen_no )
{
#ifdef HAS_XINERAMA
	XineramaScreenInfo *screeninfo = NULL;
	int screens;
	int event_basep, error_basep, a, b;
	CNFGDisplay = XOpenDisplay(NULL);
	int screen = XDefaultScreen(CNFGDisplay);
	int xpos, ypos;

	if (!XShapeQueryExtension(CNFGDisplay, &event_basep, &error_basep)) {
		fprintf( stderr, "X-Server does not support shape extension\n" );
		exit( 1 );
	}

 	CNFGVisual = DefaultVisual(CNFGDisplay, screen);
	CNFGWinAtt.depth = DefaultDepth(CNFGDisplay, screen);

#ifdef CNFGOGL
	int attribs[] = { GLX_RGBA,
		GLX_DOUBLEBUFFER, 
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DEPTH_SIZE, 1,
		None };
	XVisualInfo * vis = glXChooseVisual(CNFGDisplay, screen, attribs);
	CNFGVisual = vis->visual;
	CNFGWinAtt.depth = vis->depth;
	CNFGCtx = glXCreateContext( CNFGDisplay, vis, NULL, True );
#endif

	if (XineramaQueryExtension(CNFGDisplay, &a, &b ) &&
		(screeninfo = XineramaQueryScreens(CNFGDisplay, &screens)) &&
		XineramaIsActive(CNFGDisplay) && screen_no >= 0 &&
		screen_no < screens ) {

		CNFGWinAtt.width = screeninfo[screen_no].width;
		CNFGWinAtt.height = screeninfo[screen_no].height;
		xpos = screeninfo[screen_no].x_org;
		ypos = screeninfo[screen_no].y_org;
	} else
	{
		CNFGWinAtt.width = XDisplayWidth(CNFGDisplay, screen);
		CNFGWinAtt.height = XDisplayHeight(CNFGDisplay, screen);
		xpos = 0;
		ypos = 0;
	}
	if (screeninfo)
	XFree(screeninfo);


	XSetWindowAttributes setwinattr;
	setwinattr.override_redirect = 1;
	setwinattr.save_under = 1;
#ifdef CNFG_HAS_XSHAPE

	if (prepare_xshape && !XShapeQueryExtension(CNFGDisplay, &event_basep, &error_basep))
	{
    	fprintf( stderr, "X-Server does not support shape extension" );
		exit( 1 );
	}

	setwinattr.event_mask = 0;
#else
	//This code is probably made irrelevant by the XSetEventMask in InternalLinkScreenAndGo, if this code is not found needed by 2019-12-31, please remove.
	//setwinattr.event_mask = StructureNotifyMask | SubstructureNotifyMask | ExposureMask | ButtonPressMask | ButtonReleaseMask | ButtonPressMask | PointerMotionMask | ButtonMotionMask | EnterWindowMask | LeaveWindowMask |KeyPressMask |KeyReleaseMask | SubstructureNotifyMask | FocusChangeMask;
#endif
	setwinattr.border_pixel = 0;
	setwinattr.colormap = XCreateColormap( CNFGDisplay, RootWindow(CNFGDisplay, 0), CNFGVisual, AllocNone);

	CNFGWindow = XCreateWindow(CNFGDisplay, XRootWindow(CNFGDisplay, screen),
		xpos, ypos, CNFGWinAtt.width, CNFGWinAtt.height,
		0, CNFGWinAtt.depth, InputOutput, CNFGVisual, 
		CWBorderPixel/* | CWEventMask */ | CWOverrideRedirect | CWSaveUnder | CWColormap, 
		&setwinattr);

	FullScreen = 1;
	InternalLinkScreenAndGo( WindowName );
#ifdef CNFGOGL
	glXMakeCurrent( CNFGDisplay, CNFGWindow, CNFGCtx );
#endif

#else
	CNFGSetup( WindowName, 640, 480 );
#endif
}


void CNFGTearDown()
{
	HandleDestroy();
	if( xi ) free( xi );
	if ( CNFGClassHint ) XFree( CNFGClassHint );
	if ( CNFGGC ) XFreeGC( CNFGDisplay, CNFGGC );
	if ( CNFGWindowGC ) XFreeGC( CNFGDisplay, CNFGWindowGC );
	if ( CNFGDisplay ) XCloseDisplay( CNFGDisplay );
	CNFGDisplay = NULL;
	CNFGWindowGC = CNFGGC = NULL;
	CNFGClassHint = NULL;
}

int CNFGSetupWMClass( const char * WindowName, int w, int h , char * wm_res_name_ , char * wm_res_class_ )
{
	wm_res_name = wm_res_name_;
	wm_res_class = wm_res_class_;
	return CNFGSetup( WindowName, w, h);
}

int CNFGSetup( const char * WindowName, int w, int h )
{
	CNFGDisplay = XOpenDisplay(NULL);
	if ( !CNFGDisplay ) {
		fprintf( stderr, "Could not get an X Display.\n%s", 
				 "Are you in text mode or using SSH without X11-Forwarding?\n" );
		exit( 1 );
	}
	atexit( CNFGTearDown );

	int screen = DefaultScreen(CNFGDisplay);
	int depth = DefaultDepth(CNFGDisplay, screen);
 	CNFGVisual = DefaultVisual(CNFGDisplay, screen);
	Window wnd = DefaultRootWindow( CNFGDisplay );

#ifdef CNFGOGL
	int attribs[] = { GLX_RGBA,
		GLX_DOUBLEBUFFER, 
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DEPTH_SIZE, 1,
		None };
	XVisualInfo * vis = glXChooseVisual(CNFGDisplay, screen, attribs);
	CNFGVisual = vis->visual;
	depth = vis->depth;
	CNFGCtx = glXCreateContext( CNFGDisplay, vis, NULL, True );
#endif

	XSetWindowAttributes attr;
	attr.background_pixel = 0;
	attr.colormap = XCreateColormap( CNFGDisplay, wnd, CNFGVisual, AllocNone);
	if( w  > 0 && h > 0 )
		CNFGWindow = XCreateWindow(CNFGDisplay, wnd, 1, 1, w, h, 0, depth, InputOutput, CNFGVisual, CWBackPixel | CWColormap, &attr );
	else
	{
		if( w < 0 ) w = -w;
		if( h < 0 ) h = -h;
		CNFGWindow = XCreateWindow(CNFGDisplay, wnd, 1, 1, w, h, 0, depth, InputOutput, CNFGVisual, CWBackPixel | CWColormap, &attr );
		CNFGWindowInvisible = 1;
	}

	InternalLinkScreenAndGo( WindowName );

//Not sure of the purpose of this code - if it's still commented out after 2019-12-31 and no one knows why, please delete it.
	Atom WM_DELETE_WINDOW = XInternAtom( CNFGDisplay, "WM_DELETE_WINDOW", False );
	XSetWMProtocols( CNFGDisplay, CNFGWindow, &WM_DELETE_WINDOW, 1 );

#ifdef CNFGOGL
	glXMakeCurrent( CNFGDisplay, CNFGWindow, CNFGCtx );
#endif

#ifdef CNFG_BATCH
	CNFGSetupBatchInternal();
#endif

	return 0;
}

int CNFGHandleInput()
{
	if( !CNFGWindow ) return 0;
	static int ButtonsDown;
	XEvent report;
	
	int bKeyDirection = 1;
	while( XPending( CNFGDisplay ) )
	{
		XNextEvent( CNFGDisplay, &report );

		bKeyDirection = 1;
		switch  (report.type)
		{
		case NoExpose:
			break;
		case Expose:
			XGetWindowAttributes( CNFGDisplay, CNFGWindow, &CNFGWinAtt );
			if( CNFGPixmap ) XFreePixmap( CNFGDisplay, CNFGPixmap );
			CNFGPixmap = XCreatePixmap( CNFGDisplay, CNFGWindow, CNFGWinAtt.width, CNFGWinAtt.height, CNFGWinAtt.depth );
			if( CNFGGC ) XFreeGC( CNFGDisplay, CNFGGC );
			CNFGGC = XCreateGC(CNFGDisplay, CNFGPixmap, 0, 0);
			break;
		case KeyRelease:
		{
			bKeyDirection = 0;
			//Tricky - handle key repeats cleanly.
			if( XPending( CNFGDisplay ) )
			{
				XEvent nev;
				XPeekEvent( CNFGDisplay, &nev );
				if (nev.type == KeyPress && nev.xkey.time == report.xkey.time && nev.xkey.keycode == report.xkey.keycode )
					bKeyDirection = 2;
			}
		}
		case KeyPress:
			g_x_global_key_state = report.xkey.state;
			g_x_global_shift_key = XLookupKeysym(&report.xkey, 1);
			HandleKey( XLookupKeysym(&report.xkey, 0), bKeyDirection );
			break;
		case ButtonRelease:
			bKeyDirection = 0;
		case ButtonPress:
			HandleButton( report.xbutton.x, report.xbutton.y, report.xbutton.button, bKeyDirection );
			ButtonsDown = (ButtonsDown & (~(1<<report.xbutton.button))) | ( bKeyDirection << report.xbutton.button );

			//Intentionall fall through -- we want to send a motion in event of a button as well.
		case MotionNotify:
			HandleMotion( report.xmotion.x, report.xmotion.y, ButtonsDown>>1 );
			break;
		case ClientMessage:
			// Only subscribed to WM_DELETE_WINDOW, so return 0 to stop game loop
			return 0;
			break;
		default:
			break;
			//printf( "Event: %d\n", report.type );
		}
	}
	return 1;
}


#ifdef CNFGOGL

void   CNFGSetVSync( int vson )
{
	void (*glfn)( int );
	glfn = (void (*)( int ))CNFGGetExtension( "glXSwapIntervalMESA" );	if( glfn ) glfn( vson );
	glfn = (void (*)( int ))CNFGGetExtension( "glXSwapIntervalSGI" );	if( glfn ) glfn( vson );
	glfn = (void (*)( int ))CNFGGetExtension( "glXSwapIntervalEXT" );	if( glfn ) glfn( vson );
}

#ifdef CNFGRASTERIZER
void CNFGSwapBuffersInternal()
#else
void CNFGSwapBuffers()
#endif
{
	if( CNFGWindowInvisible ) return;

#ifndef CNFGRASTERIZER
	CNFGFlushRender();
#endif

#ifdef CNFG_HAS_XSHAPE
	if( taint_shape )
	{
		XShapeCombineMask(CNFGDisplay, CNFGWindow, ShapeBounding, 0, 0, xspixmap, ShapeSet);
		taint_shape = 0;
	}
#endif //CNFG_HAS_XSHAPE
	glXSwapBuffers( CNFGDisplay, CNFGWindow );

#ifdef FULL_SCREEN_STEAL_FOCUS
	if( FullScreen )
		XSetInputFocus( CNFGDisplay, CNFGWindow, RevertToParent, CurrentTime );
#endif //FULL_SCREEN_STEAL_FOCUS
}

#else //CNFGOGL

#ifndef CNFGRASTERIZER
void CNFGBlitImage( uint32_t * data, int x, int y, int w, int h )
{
	static int depth;
	static int lw, lh;

	if( !xi )
	{
		int screen = DefaultScreen(CNFGDisplay);
		depth = DefaultDepth(CNFGDisplay, screen)/8;
	}

	if( lw != w || lh != h )
	{
		if( xi ) free( xi );
		xi = XCreateImage(CNFGDisplay, CNFGVisual, depth*8, ZPixmap, 0, (char*)data, w, h, 32, w*4 );
		lw = w;
		lh = h;
	}

	//Draw image to pixmap (not a screen flip)
	XPutImage(CNFGDisplay, CNFGPixmap, CNFGGC, xi, 0, 0, x, y, w, h );
}
#endif

void CNFGUpdateScreenWithBitmap( uint32_t * data, int w, int h )
{
	static int depth;
	static int lw, lh;

	if( !xi )
	{
		int screen = DefaultScreen(CNFGDisplay);
		depth = DefaultDepth(CNFGDisplay, screen)/8;
//		xi = XCreateImage(CNFGDisplay, DefaultVisual( CNFGDisplay, DefaultScreen(CNFGDisplay) ), depth*8, ZPixmap, 0, (char*)data, w, h, 32, w*4 );
//		lw = w;
//		lh = h;
	}

	if( lw != w || lh != h )
	{
		if( xi ) free( xi );
		xi = XCreateImage(CNFGDisplay, CNFGVisual, depth*8, ZPixmap, 0, (char*)data, w, h, 32, w*4 );
		lw = w;
		lh = h;
	}

	//Directly write image to screen (effectively a flip)
	XPutImage(CNFGDisplay, CNFGWindow, CNFGWindowGC, xi, 0, 0, 0, 0, w, h );
}

#endif //CNFGOGL

#if !defined( CNFGOGL)
#define AGLF(x) x
#else
#define AGLF(x) static inline BACKEND_##x
#endif

#if defined( CNFGRASTERIZER ) 
#include "CNFGRasterizer.c"
#undef AGLF
#define AGLF(x) static inline BACKEND_##x
#endif

uint32_t AGLF(CNFGColor)( uint32_t RGB )
{
	CNFGLastColor = RGB;
	unsigned char red = ( RGB >> 24 ) & 0xFF;
	unsigned char grn = ( RGB >> 16 ) & 0xFF;
	unsigned char blu = ( RGB >> 8 ) & 0xFF;
	unsigned long color = (red<<16)|(grn<<8)|(blu);
	XSetForeground(CNFGDisplay, CNFGGC, color);
	return color;
}

void AGLF(CNFGClearFrame)()
{
	XGetWindowAttributes( CNFGDisplay, CNFGWindow, &CNFGWinAtt );
	XSetForeground(CNFGDisplay, CNFGGC, CNFGColor(CNFGBGColor) );	
	XFillRectangle(CNFGDisplay, CNFGPixmap, CNFGGC, 0, 0, CNFGWinAtt.width, CNFGWinAtt.height );
}

void AGLF(CNFGSwapBuffers)()
{
#ifdef CNFG_HAS_XSHAPE
	if( taint_shape )
	{
		XShapeCombineMask(CNFGDisplay, CNFGWindow, ShapeBounding, 0, 0, xspixmap, ShapeSet);
		taint_shape = 0;
	}
#endif
	XCopyArea(CNFGDisplay, CNFGPixmap, CNFGWindow, CNFGWindowGC, 0,0,CNFGWinAtt.width,CNFGWinAtt.height,0,0);
	XFlush(CNFGDisplay);
#ifdef FULL_SCREEN_STEAL_FOCUS
	if( FullScreen )
		XSetInputFocus( CNFGDisplay, CNFGWindow, RevertToParent, CurrentTime );
#endif
}

void AGLF(CNFGTackSegment)( short x1, short y1, short x2, short y2 )
{
	if( x1 == x2 && y1 == y2 )
	{
		//On some targets, zero-length lines will not show up.
		//This is tricky - since this will also cause more draw calls for points on systems like GLAMOR.
		XDrawPoint( CNFGDisplay, CNFGPixmap, CNFGGC, x2, y2 );
	}
	else
	{
		//XXX HACK!  See discussion here: https://github.com/cntools/cnping/issues/68
		XDrawLine( CNFGDisplay, CNFGPixmap, CNFGGC, x1, y1, x2, y2 );
		XDrawLine( CNFGDisplay, CNFGPixmap, CNFGGC, x2, y2, x1, y1 );
	}
}

void AGLF(CNFGTackPixel)( short x1, short y1 )
{
	XDrawPoint( CNFGDisplay, CNFGPixmap, CNFGGC, x1, y1 );
}

void AGLF(CNFGTackRectangle)( short x1, short y1, short x2, short y2 )
{
	XFillRectangle(CNFGDisplay, CNFGPixmap, CNFGGC, x1, y1, x2-x1, y2-y1 );
}

void AGLF(CNFGTackPoly)( RDPoint * points, int verts )
{
	XFillPolygon(CNFGDisplay, CNFGPixmap, CNFGGC, (XPoint *)points, verts, Convex, CoordModeOrigin );
}

void AGLF(CNFGInternalResize)( short x, short y ) { }

void AGLF(CNFGSetLineWidth)( short width )
{
	XSetLineAttributes(CNFGDisplay, CNFGGC, width, LineSolid, CapRound, JoinRound);
}

#endif // _CNFGXDRIVER_C

