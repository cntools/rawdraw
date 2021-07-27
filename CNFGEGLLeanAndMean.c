//Copyright (c) 2011, 2017, 2018, 2020 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.

//This driver cannot create an OpenGL Surface, but can be used for computing for background tasks.

//NOTE: This is a truly incomplete driver - if no EGL surface is available, it does not support direct buffer rendering.
//Additionally no input is connected.

#include "CNFG.h"

#include <stdio.h>
#include <stdlib.h>

#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2ext.h>


static const EGLint configAttribs[] = {
	EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_NONE
};

EGLint context_attribs[] = { 
	EGL_CONTEXT_CLIENT_VERSION, 2, 
	EGL_NONE 
};

static int pbufferWidth = 0;
static int pbufferHeight = 0;

static EGLint pbufferAttribs[] = {
	EGL_WIDTH, 0,
	EGL_HEIGHT, 0,
	EGL_NONE,
};

EGLDisplay eglDpy = 0;
EGLContext eglCtx = 0;
EGLSurface eglSurf = 0;

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
	//Fullscreen is meaningless for this driver, since it doesn't really open a window.
	CNFGSetup( WindowName, 1024, 1024 );
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
	eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	atexit( CNFGTearDown );
	printf( "EGL Display: %p\n", eglDpy );

	pbufferAttribs[1] = pbufferWidth = w;
	pbufferAttribs[3] = pbufferHeight = h;

	EGLint major, minor;
	eglInitialize(eglDpy, &major, &minor);

	EGLint numConfigs=0;
	EGLConfig eglCfg=NULL;

    eglChooseConfig(eglDpy, configAttribs, 0, 0, &numConfigs); //this gets number of configs
	if (numConfigs) {
		eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
		printf( " EGL config found\n" );
	} else {
		printf( " Error could not find a valid config avail.. \n" );
	}

	printf( "EGL Major Minor: %d %d\n", major, minor );
	eglBindAPI(EGL_OPENGL_API);
	eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, context_attribs);
	int err = eglGetError(); if(err != EGL_SUCCESS) { printf("1. Error %d\n", err); }
	printf( "EGL Got context: %p\n", eglCtx );

	if( w > 0 && h > 0 )
	{
		eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);
		eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);
		printf( "EGL Current, with surface %p\n", eglSurf );
		//Actually have a surface.  Need to allocate it.
		EGLint surfwid;
		EGLint surfht;
		eglQuerySurface(eglDpy, eglSurf, EGL_WIDTH, &surfwid);
		eglQuerySurface(eglDpy, eglSurf, EGL_HEIGHT, &surfht);
		printf("Window dimensions: %d x %d\n", surfwid, surfht);
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

void CNFGSetVSync( int vson )
{
	//No-op
}

void CNFGSwapBuffers()
{
	//No-op
}

