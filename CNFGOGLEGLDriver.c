//Copyright (c) 2011, 2017, 2018 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.
//portions from 
//https://github.com/NVIDIA-developer-blog/code-samples/blob/master/posts/egl_OpenGl_without_Xserver/tinyegl.cc

//NOTE: This is a truly incomplete driver - if no EGL surface is available, it does not support direct buffer rendering.
//Additionally no input is connected.

#include "CNFG.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef USE_EGL_GET_DISPLAY
#include <EGL/egl.h>
#else
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#ifdef USE_EGL_SURFACE
#include <GL/gl.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif


static const EGLint configAttribs[] = {
	EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
	EGL_RED_SIZE, 1,
	EGL_GREEN_SIZE, 1,
	EGL_BLUE_SIZE, 1,
	EGL_DEPTH_SIZE, 1,
	EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
	EGL_NONE
};

static int pbufferWidth = 0;
static int pbufferHeight = 0;
static EGLint pbufferAttribs[] = {
	EGL_WIDTH, 0,
	EGL_HEIGHT, 0,
	EGL_NONE,
};

#ifdef CNFGOGL
#include <GL/glx.h>
#include <GL/glxext.h>
GLXContext CNFGCtx;
void * CNFGGetExtension( const char * extname ) { return glXGetProcAddressARB((const GLubyte *) extname); }
#endif

EGLDisplay eglDpy = 0;
EGLContext eglCtx = 0;
EGLSurface eglSurf = 0;

//These aren't actually used.  Just here to allow linking.
XWindowAttributes CNFGWinAtt;
XClassHint *CNFGClassHint;
Display *CNFGDisplay;
Window CNFGWindow;
int CNFGWindowInvisible;
Pixmap CNFGPixmap;
GC     CNFGGC;
GC     CNFGWindowGC;
Visual * CNFGVisual;

int FullScreen = 0;

void CNFGGetDimensions( short * x, short * y )
{
	*x = pbufferWidth;
	*y = pbufferHeight;
}

void	CNFGChangeWindowTitle( const char * WindowName )
{
}

void CNFGSetupFullscreen( const char * WindowName, int screen_no )
{
	printf( "Full screen setup stubbed.\n" );
	CNFGSetup( WindowName, 640, 480 );
}

void CNFGTearDown()
{
	if( eglDpy )
	{
		eglTerminate( eglDpy );
	}
	//Unimplemented.
}

int CNFGSetup( const char * WindowName, int w, int h )
{
//	glXMakeCurrent( CNFGDisplay, CNFGWindow, CNFGCtx );
	eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	atexit( CNFGTearDown );
	printf( "EGL Display: %p\n", eglDpy );
	if( eglDpy == 0 )
	{
		printf( "Trying alternate method.\n" );
		// obtain display by specifying an appropriate device. This is the preferred method today.
		// load the function pointers for the device,platform extensions
		PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
				   (PFNEGLQUERYDEVICESEXTPROC) eglGetProcAddress("eglQueryDevicesEXT");
		if(!eglQueryDevicesEXT) {
			 printf("ERROR: extension eglQueryDevicesEXT not available");
			 return(-1);
		}

		PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
				   (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

		if(!eglGetPlatformDisplayEXT) {
			 printf("ERROR: extension eglGetPlatformDisplayEXT not available");
			 return(-1);
		}

		static const int MAX_DEVICES = 16;
		EGLDeviceEXT devices[MAX_DEVICES];
		EGLint numDevices;

		eglQueryDevicesEXT(MAX_DEVICES, devices, &numDevices);

		eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, devices[0], 0);
		printf( "EGL Backup Display: %p\n", eglDpy );
	}


	pbufferAttribs[1] = pbufferWidth = w;
	pbufferAttribs[3] = pbufferHeight = h;

	EGLint major, minor;
	eglInitialize(eglDpy, &major, &minor);

	EGLint numConfigs;
	EGLConfig eglCfg;
	eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
	printf( "EGL Major Minor: %d %d\n", major, minor );
	eglBindAPI(EGL_OPENGL_API);
	eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);
	int err = eglGetError(); if(err != EGL_SUCCESS) { printf("1. Error %d\n", err); }
	printf( "EGL Got context: %p\n", eglCtx );

	if( w > 0 && h > 0 )
	{
		eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);
		eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
		printf( "EGL Current, with surface %p\n", eglSurf );
		//Actually have a surface.  Need to allocate it.
	}
	else
	{
		eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, eglCtx);
		printf( "EGL Current, no surface.\n" );
	}
	return 0;
}

int CNFGHandleInput()
{
	//Stubbed (No input)
	return 1;
}
void CNFGUpdateScreenWithBitmap( unsigned long * data, int w, int h )
{
	printf( "Stubbed update screen\n" );
}
void CNFGSetVSync( int vson )
{
	//No-op
}

void CNFGSwapBuffers()
{
	//No-op
}


