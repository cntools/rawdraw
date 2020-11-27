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

#ifdef CNFGCONTEXTONLY
void CNFGInternalResize( short x, short y ) { }
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
	if( w && h )
		CNFGWindow = XCreateWindow(CNFGDisplay, wnd, 1, 1, w, h, 0, depth, InputOutput, CNFGVisual, CWBackPixel | CWColormap, &attr );
	else
	{
		CNFGWindow = XCreateWindow(CNFGDisplay, wnd, 1, 1, 1, 1, 0, depth, InputOutput, CNFGVisual, CWBackPixel | CWColormap, &attr );
		CNFGWindowInvisible = 1;
	}

	InternalLinkScreenAndGo( WindowName );

//Not sure of the purpose of this code - if it's still commented out after 2019-12-31 and no one knows why, please delete it.
//	Atom WM_DELETE_WINDOW = XInternAtom( CNFGDisplay, "WM_DELETE_WINDOW", False );
//	XSetWMProtocols( CNFGDisplay, CNFGWindow, &WM_DELETE_WINDOW, 1 );

#ifdef CNFGOGL
	glXMakeCurrent( CNFGDisplay, CNFGWindow, CNFGCtx );
#endif

#ifdef CNFG_BATCH
	CNFGSetupBatchInternal();
#endif

	return 0;
}

void CNFGHandleInput()
{
	if( !CNFGWindow ) return;
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
			// Only subscribed to WM_DELETE_WINDOW, so just exit
			exit( 0 );
			break;
		default:
			break;
			//printf( "Event: %d\n", report.type );
		}
	}
}


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

	XPutImage(CNFGDisplay, CNFGWindow, CNFGWindowGC, xi, 0, 0, 0, 0, w, h );
}



#ifdef CNFGOGL

void   CNFGSetVSync( int vson )
{
	void (*glfn)( int );
	glfn = (void (*)( int ))CNFGGetExtension( "glXSwapIntervalMESA" );	if( glfn ) glfn( vson );
	glfn = (void (*)( int ))CNFGGetExtension( "glXSwapIntervalSGI" );	if( glfn ) glfn( vson );
	glfn = (void (*)( int ))CNFGGetExtension( "glXSwapIntervalEXT" );	if( glfn ) glfn( vson );
}

void CNFGSwapBuffers()
{
	if( CNFGWindowInvisible ) return;

	CNFGFlushRender();

#ifdef CNFG_HAS_XSHAPE
	if( taint_shape )
	{
		XShapeCombineMask(CNFGDisplay, CNFGWindow, ShapeBounding, 0, 0, xspixmap, ShapeSet);
		taint_shape = 0;
	}
#endif
	glXSwapBuffers( CNFGDisplay, CNFGWindow );

#ifdef FULL_SCREEN_STEAL_FOCUS
	if( FullScreen )
		XSetInputFocus( CNFGDisplay, CNFGWindow, RevertToParent, CurrentTime );
#endif
}
#endif

#if !defined( CNFGOGL)
#define AGLF(x) x
#else
#define AGLF(x) static inline BACKEND_##x
#if defined( CNFGRASTERIZER ) 
#include "CNFGRasterizer.c"
#endif
#endif

uint32_t AGLF(CNFGColor)( uint32_t RGB )
{
	unsigned char red = RGB & 0xFF;
	unsigned char grn = ( RGB >> 8 ) & 0xFF;
	unsigned char blu = ( RGB >> 16 ) & 0xFF;
	CNFGLastColor = RGB;
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
	XFillPolygon(CNFGDisplay, CNFGPixmap, CNFGGC, (XPoint *)points, 3, Convex, CoordModeOrigin );
}

void AGLF(CNFGInternalResize)( short x, short y ) { }

void AGLF(CNFGSetLineWidth)( short width )
{
	XSetLineAttributes(CNFGDisplay, CNFGGC, width, LineSolid, CapRound, JoinRound);
}


#ifdef CNFG_BATCH

#define CNFGOGL_NEED_EXTENSION

#ifdef  CNFGOGL_NEED_EXTENSION
//If we are going to be defining our own function pointer call
#define CHEWTYPEDEF( ret, name, rv, paramcall, ... ) \
	ret (*CNFG##name)( __VA_ARGS__ );
#else
//If we are going to be defining the real call
#define CHEWTYPEDEF( ret, name, rv, paramcall, ... ) \
	ret name (__VA_ARGS__);
#endif

CHEWTYPEDEF( GLint, glGetUniformLocation, return, (program,name), GLuint program, const GLchar *name )
CHEWTYPEDEF( void, glEnableVertexAttribArray, , (index), GLuint index )
CHEWTYPEDEF( void, glUseProgram, , (program), GLuint program )
CHEWTYPEDEF( void, glGetProgramInfoLog, , (program,maxLength, length, infoLog), GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog )
CHEWTYPEDEF( void, glGetProgramiv, , (program,pname,params), GLuint program, GLenum pname, GLint *params )
CHEWTYPEDEF( void, glBindAttribLocation, , (program,index,name), GLuint program, GLuint index, const GLchar *name )
CHEWTYPEDEF( void, glGetShaderiv, , (shader,pname,params), GLuint shader, GLenum pname, GLint *params )
CHEWTYPEDEF( GLuint, glCreateShader, return, (e), GLenum e )
CHEWTYPEDEF( void, glVertexAttribPointer, , (index,size,type,normalized,stride,pointer), GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer )
CHEWTYPEDEF( void, glShaderSource, , (shader,count,string,length), GLuint shader, GLsizei count, const GLchar **string, const GLint *length )
CHEWTYPEDEF( void, glAttachShader, , (program,shader), GLuint program, GLuint shader )
CHEWTYPEDEF( void, glCompileShader, ,(shader), GLuint shader )
CHEWTYPEDEF( void, glGetShaderInfoLog , , (shader,maxLength, length, infoLog), GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog )
CHEWTYPEDEF( GLuint, glCreateProgram, return, () , void )
CHEWTYPEDEF( void, glLinkProgram, , (program), GLuint program )
CHEWTYPEDEF( void, glDeleteShader, , (shader), GLuint shader )
CHEWTYPEDEF( void, glUniform4f, , (location,v0,v1,v2,v3), GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )


#ifndef CNFGOGL_NEED_EXTENSION
#define CNFGglGetUniformLocation glGetUniformLocation
#define CNFGglEnableVertexAttribArray glEnableVertexAttribArray
#define CNFGglUseProgram glUseProgram
#define CNFGglEnableVertexAttribArray glEnableVertexAttribArray
#define CNFGglUseProgram glUseProgram
#define CNFGglGetProgramInfoLog glGetProgramInfoLog
#define CNFGglGetProgramiv glGetProgramiv
#define CNFGglShaderSource glShaderSource
#define CNFGglCreateShader glCreateShader
#define CNFGglAttachShader glAttachShader
#define CNFGglGetShaderiv glGetShaderiv
#define CNFGglCompileShader glCompileShader
#define CNFGglGetShaderInfoLog glGetShaderInfoLog
#define CNFGglCreateProgram glCreateProgram
#define CNFGglLinkProgram glLinkProgram
#define CNFGglDeleteShader glDeleteShader
#define CNFGglUniform4f glUniform4f
#define CNFGglBindAttribLocation glBindAttribLocation
#define CNFGglVertexAttribPointer glVertexAttribPointer
#endif

#ifdef CNFGOGL_NEED_EXTENSION
#if defined( WIN32 ) || defined( WINDOWS ) || defined( WIN64 )

//From https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
void * CNFGGetProcAddress(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);
	if(p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1) )
	{
		static HMODULE module;
		if( !module ) module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}
	return p;
}

#else
#include <dlfcn.h>


void * CNFGGetProcAddress(const char *name)
{
	//Tricky use RTLD_NEXT first so we don't accidentally link against ourselves.
	void * v1 = dlsym( (void*)((intptr_t)-1) /*RTLD_NEXT = -1*/ /*RTLD_DEFAULT = 0*/, name );
	//printf( "%s = %p\n", name, v1 );
	if( !v1 ) v1 = dlsym( 0, name );
	return v1;
}

#endif

static void CNFGLoadExtensionsInternal()
{
	CNFGglGetUniformLocation = CNFGGetProcAddress( "glGetUniformLocation" );
	CNFGglEnableVertexAttribArray = CNFGGetProcAddress( "glEnableVertexAttribArray" );
	CNFGglUseProgram = CNFGGetProcAddress( "glUseProgram" );
	CNFGglGetProgramInfoLog = CNFGGetProcAddress( "glGetProgramInfoLog" );
	CNFGglBindAttribLocation = CNFGGetProcAddress( "glBindAttribLocation" );
	CNFGglGetProgramiv = CNFGGetProcAddress( "glGetProgramiv" );
	CNFGglGetShaderiv = CNFGGetProcAddress( "glGetShaderiv" );
	CNFGglVertexAttribPointer = CNFGGetProcAddress( "glVertexAttribPointer" );
	CNFGglCreateShader = CNFGGetProcAddress( "glCreateShader" );
	CNFGglVertexAttribPointer = CNFGGetProcAddress( "glVertexAttribPointer" );
	CNFGglShaderSource = CNFGGetProcAddress( "glShaderSource" );
	CNFGglAttachShader = CNFGGetProcAddress( "glAttachShader" );
	CNFGglCompileShader = CNFGGetProcAddress( "glCompileShader" );
	CNFGglGetShaderInfoLog = CNFGGetProcAddress( "glGetShaderInfoLog" );
	CNFGglLinkProgram = CNFGGetProcAddress( "glLinkProgram" );
	CNFGglDeleteShader = CNFGGetProcAddress( "glDeleteShader" );
	CNFGglUniform4f = CNFGGetProcAddress( "glUniform4f" );
	CNFGglCreateProgram = CNFGGetProcAddress( "glCreateProgram" );
}
#else
static void CNFGLoadExtensionsInternal() { }
#endif



GLuint gRDShaderProg = -1;
GLuint gRDBlitProg = -1;
GLuint gRDShaderProgUX = -1;
GLuint gRDBlitProgUX = -1;
GLuint gRDBlitProgUT = -1;
GLuint gRDBlitProgTex = -1;
GLuint gRDLastResizeW;
GLuint gRDLastResizeH;


GLuint CNFGGLInternalLoadShader( const char * vertex_shader, const char * fragment_shader )
{
	GLuint fragment_shader_object = 0;
	GLuint vertex_shader_object = 0;
	GLuint program = 0;
	int ret;

	vertex_shader_object = CNFGglCreateShader(GL_VERTEX_SHADER);
	if (!vertex_shader_object) {
		fprintf( stderr, "Error: glCreateShader(GL_VERTEX_SHADER) "
			"failed: 0x%08X\n", glGetError());
		goto fail;
	}

	CNFGglShaderSource(vertex_shader_object, 1, &vertex_shader, NULL);
	CNFGglCompileShader(vertex_shader_object);

	CNFGglGetShaderiv(vertex_shader_object, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf( stderr,"Error: vertex shader compilation failed!\n");
		CNFGglGetShaderiv(vertex_shader_object, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			CNFGglGetShaderInfoLog(vertex_shader_object, ret, NULL, log);
			fprintf( stderr, "%s", log);
		}
		goto fail;
	}

	fragment_shader_object = CNFGglCreateShader(GL_FRAGMENT_SHADER);
	if (!fragment_shader_object) {
		fprintf( stderr, "Error: glCreateShader(GL_FRAGMENT_SHADER) "
			"failed: 0x%08X\n", glGetError());
		goto fail;
	}

	CNFGglShaderSource(fragment_shader_object, 1, &fragment_shader, NULL);
	CNFGglCompileShader(fragment_shader_object);

	CNFGglGetShaderiv(fragment_shader_object, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf( stderr, "Error: fragment shader compilation failed!\n");
		CNFGglGetShaderiv(fragment_shader_object, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			CNFGglGetShaderInfoLog(fragment_shader_object, ret, NULL, log);
			fprintf( stderr, "%s", log);
		}
		goto fail;
	}

	program = CNFGglCreateProgram();
	if (!program) {
		fprintf( stderr, "Error: failed to create program!\n");
		goto fail;
	}

	CNFGglAttachShader(program, vertex_shader_object);
	CNFGglAttachShader(program, fragment_shader_object);

	CNFGglBindAttribLocation(program, 0, "a0");
	CNFGglBindAttribLocation(program, 1, "a1");

	CNFGglLinkProgram(program);

	CNFGglGetProgramiv(program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf( stderr, "Error: program linking failed!\n");
		CNFGglGetProgramiv(program, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			CNFGglGetProgramInfoLog(program, ret, NULL, log);
			fprintf( stderr, "%s", log);
		}
		goto fail;
	}
	return program;
fail:
	if( !vertex_shader_object ) CNFGglDeleteShader( vertex_shader_object );
	if( !fragment_shader_object ) CNFGglDeleteShader( fragment_shader_object );
	if( !program ) CNFGglDeleteShader( program );
	return -1;
}


void CNFGSetupBatchInternal()
{
	short w, h;
	printf( "SETUP INTERNAL\n" );
	CNFGLoadExtensionsInternal();

	CNFGGetDimensions( &w, &h );

	gRDShaderProg = CNFGGLInternalLoadShader( 
		"uniform vec4 xfrm; attribute vec3 a0; attribute vec4 a1; varying vec4 vc; void main() { gl_Position = vec4( a0.xy*xfrm.xy+xfrm.zw, a0.z, 0.5 ); vc = a1; }",
		"precision mediump float; varying vec4 vc; void main() { gl_FragColor = vec4(vc.xyzw); }" );

	CNFGglUseProgram( gRDShaderProg );
	gRDShaderProgUX = CNFGglGetUniformLocation ( gRDShaderProg , "xfrm" );


	gRDBlitProg = CNFGGLInternalLoadShader( 
		"uniform vec4 xfrm; attribute vec3 a0; attribute vec4 a1; varying vec2 tc; void main() { gl_Position = vec4( a0.xy*xfrm.xy+xfrm.zw, a0.z, 0.5 ); tc = a1.xy; }",
		"precision mediump float; varying vec2 tc; uniform sampler2D tex; void main() { gl_FragColor = texture2D(tex,tc);}" );

	CNFGglUseProgram( gRDBlitProg );
	gRDBlitProgUX = CNFGglGetUniformLocation ( gRDBlitProg , "xfrm" );
	gRDBlitProgUT = CNFGglGetUniformLocation ( gRDBlitProg , "tex" );
	glGenTextures( 1, &gRDBlitProgTex );

	CNFGVertPlace = 0;
}

void CNFGInternalResize(short x, short y)
{
	glViewport( 0, 0, x, y );
	gRDLastResizeW = x;
	gRDLastResizeH = y;
	CNFGglUseProgram( gRDShaderProg );
	CNFGglUniform4f( gRDShaderProgUX, 1.f/x, -1.f/y, -0.5f, 0.5f);
	printf( "Internal Resize\n" );
}

void CNFGFlushRender()
{
	CNFGglUseProgram( gRDShaderProg );
	CNFGglUniform4f( gRDShaderProgUX, 1.f/gRDLastResizeW, -1.f/gRDLastResizeH, -0.5f, 0.5f);
	CNFGglVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, CNFGVertDataV);
	CNFGglEnableVertexAttribArray(0);
	CNFGglVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, CNFGVertDataC);
	CNFGglEnableVertexAttribArray(1);
	glDrawArrays( GL_TRIANGLES, 0, CNFGVertPlace);
	//printf( "CNFGVertPlace = %d %f %f %f\n", CNFGVertPlace, CNFGVertDataV[0],CNFGVertDataV[1], CNFGVertDataV[2] );
	CNFGVertPlace = 0;
}

void CNFGClearFrame()
{
	glClearColor( (CNFGBGColor&0xff)/255.0, 
		(CNFGBGColor&0xff00)/65280.0, 
		(CNFGBGColor&0xff0000)/16711680.0,
		((CNFGBGColor&0xff000000)>>24)/255.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

#else

void CNFGFlushRender() { }

#endif

#endif // _CNFGXDRIVER_C

