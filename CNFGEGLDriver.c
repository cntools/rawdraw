/*
 * Copyright (c) 2011-2013 Luc Verhaegen <libv@skynet.be>
 * Copyright (c) 2018 <>< Charles Lohr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

//Note: This interface provides the following two things privately.
//you may "extern" them in your code.
void FlushRender();
#ifdef ANDROID
void AndroidDisplayKeyboard(int pShow);
int AndroidGetUnicodeChar( int keyCode, int metaState );
struct android_app * gapp;
int android_width, android_height;
static const char* kTAG;

//You must implement these.
void HandleResume();
void HandleSuspend();
#endif





#include "CNFGFunctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <EGL/egl.h>

#ifdef ANDROID
#include <GLES3/gl3.h>
#else
#include <GLES2/gl2.h>
#endif

#ifdef ANDROID
#include <android_native_app_glue.h>
#include <android/log.h>
#include <jni.h>
#include <native_activity.h>
#define KTAG( x ) #x
static const char* kTAG = KTAG(APPNAME);
#define LOGI(...)  ((void)__android_log_print(ANDROID_LOG_INFO, kTAG, __VA_ARGS__))
#define printf( x...) LOGI( x )
#endif

#define EGL_ZBITS 16
#define EGL_IMMEDIATE_SIZE 2048

/*
	Just FYI If running on the rpi, make sure you have the right mode set.

		vcgencmd hdmi_timings 2160 1 40 20 46 1200 1 28 2 234 0 0 0 90 0 297000000 5 && tvservice -e "DMT 87"
		sleep .5
		fbset -depth 8 && fbset -depth 16
		sleep .5
*/


#ifdef RASPI_GPU
struct rpi_state_t
{
	int w, h;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_ELEMENT_HANDLE_T dispman_element;
} rpi_state;
#endif

#ifdef USE_EGL_X
	#error This feature has never been completed or tested.
	Display *XDisplay;
	Window XWindow;
#else
	typedef enum
	{
		FBDEV_PIXMAP_DEFAULT = 0,
		FBDEV_PIXMAP_SUPPORTS_UMP = (1<<0),
		FBDEV_PIXMAP_ALPHA_FORMAT_PRE = (1<<1),
		FBDEV_PIXMAP_COLORSPACE_sRGB = (1<<2),
		FBDEV_PIXMAP_EGL_MEMORY = (1<<3)        /* EGL allocates/frees this memory */
	} fbdev_pixmap_flags;

	typedef struct fbdev_window
	{
		unsigned short width;
		unsigned short height;
	} fbdev_window;

	typedef struct fbdev_pixmap
	{
		unsigned int height;
		unsigned int width;
		unsigned int bytes_per_pixel;
		unsigned char buffer_size;
		unsigned char red_size;
		unsigned char green_size;
		unsigned char blue_size;
		unsigned char alpha_size;
		unsigned char luminance_size;
		fbdev_pixmap_flags flags;
		unsigned short *data;
		unsigned int format; /* extra format information in case rgbal is not enough, especially for YUV formats */
	} fbdev_pixmap;
#ifdef RASPI_GPU
static EGL_DISPMANX_WINDOW_T native_window;
#elif defined( ANDROID )
EGLNativeWindowType native_window;
#else
struct fbdev_window native_window;
#endif
#endif



static const char *default_vertex_shader_source =
	"attribute vec4 aPosition;    \n"
	"attribute vec4 aColor;       \n"
	"uniform vec4 screenscale;    \n"
	"                             \n"
	"varying vec4 vColor;         \n"
	"                             \n"
	"void main()                  \n"
	"{                            \n"
	"    vColor = aColor;         \n"
	"    gl_Position = vec4( -1.0, 1.0, 0.0, 0.0 ) + aPosition * screenscale; \n"
	"}                            \n";
static const char *default_fragment_shader_source =
	"precision mediump float;     \n"
	"                             \n"
	"varying vec4 vColor;         \n"
	"                             \n"
	"void main()                  \n"
	"{                            \n"
	"    gl_FragColor = vColor;   \n"
	"}                            \n";
GLuint default_vertex_shader;
GLuint default_fragment_shader;
GLuint default_screenscale_offset;


static EGLint const config_attribute_list[] = {
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_ALPHA_SIZE, 8,
	EGL_BUFFER_SIZE, 32,
	EGL_STENCIL_SIZE, 0,
	EGL_DEPTH_SIZE, EGL_ZBITS,
	//EGL_SAMPLES, 1,
#ifdef ANDROID
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
#else
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PIXMAP_BIT,
#endif
	EGL_NONE
};


static EGLint window_attribute_list[] = {
	EGL_NONE
};

static const EGLint context_attribute_list[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};

EGLDisplay egl_display;
EGLSurface egl_surface;
uint32_t egl_currentcolor;

uint32_t CNFGColor( uint32_t RGB ) { return( egl_currentcolor = RGB|((RGB<0x1000000)?0xff000000:0) ); }

void CNFGUpdateScreenWithBitmap( unsigned long * data, int w, int h )
{
	fprintf( stderr, "Screen bitmap update not permitted.\n" );
	//Not implemented
}



int egl_immediate_stride;
int egl_immediate_size;
int egl_immediate_draw_mode;
int16_t         egl_immediate_geo_buffer[EGL_IMMEDIATE_SIZE*4];
int16_t	*	 egl_immediate_geo_ptr;
uint32_t 		 egl_immediate_color_buffer[EGL_IMMEDIATE_SIZE*4];


static GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
			       -0.5f, -0.5f, 0.0f,
				0.5f, -0.5f, 0.0f };
static GLfloat vColors[] = {1.0f, 0.0f, 0.0f, 1.0f,
			    0.0f, 1.0f, 0.0f, 1.0f,
			    0.0f, 0.0f, 1.0f, 1.0f};


void FlushRender()
{
	if( egl_immediate_size && egl_immediate_draw_mode >= 0 )
	{
		//printf( "%d %d %d %d\n",egl_immediate_geo_buffer[0],egl_immediate_geo_buffer[1], egl_immediate_geo_buffer[2], egl_immediate_geo_buffer[3]  );
		//printf( "%d*%d ", egl_immediate_size,egl_immediate_stride );
		glVertexAttribPointer(0, egl_immediate_stride, GL_SHORT, GL_FALSE, 0, egl_immediate_geo_buffer);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, egl_immediate_color_buffer);
		glEnableVertexAttribArray(1);
		glDrawArrays(egl_immediate_draw_mode, 0, egl_immediate_size);
	
		egl_immediate_size = 0;
	}

	egl_immediate_geo_ptr = egl_immediate_geo_buffer;
	egl_immediate_draw_mode = -1;
}

void CNFGTackPixel( short x1, short y1 )
{
	if( egl_immediate_draw_mode != GL_POINTS 
		|| (egl_immediate_size+2) >= EGL_IMMEDIATE_SIZE )	FlushRender();
	egl_immediate_geo_ptr[0] = x1;
	egl_immediate_geo_ptr[1] = y1;
	egl_immediate_geo_ptr += 2;
	egl_immediate_color_buffer[egl_immediate_size] = egl_currentcolor;
	egl_immediate_size++;
	egl_immediate_draw_mode = GL_POINTS;
	egl_immediate_stride = 2;
}

void CNFGTackSegment( short x1, short y1, short x2, short y2 )
{
	if( egl_immediate_draw_mode != GL_LINES 
		|| egl_immediate_size >= EGL_IMMEDIATE_SIZE )	FlushRender();

	int ofsxy1 = 0;
	if( x1 == x2 && y1 == y2 )
	{
		ofsxy1 = 1;
	}

	egl_immediate_geo_ptr[0] = x1;
	egl_immediate_geo_ptr[1] = y1;
	egl_immediate_geo_ptr[2] = x2;
	egl_immediate_geo_ptr[3] = y2+ofsxy1;
	egl_immediate_geo_ptr[4] = x2+ofsxy1;
	egl_immediate_geo_ptr[5] = y2;
	egl_immediate_geo_ptr[6] = x1;
	egl_immediate_geo_ptr[7] = y1;
	egl_immediate_geo_ptr += 8;
	egl_immediate_color_buffer[egl_immediate_size+0] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+1] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+2] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+3] = egl_currentcolor;
	egl_immediate_size+=4;

	egl_immediate_draw_mode = GL_LINES;
	egl_immediate_stride = 2;
}

void CNFGTackRectangle( short x1, short y1, short x2, short y2 )
{
	if( egl_immediate_draw_mode != GL_TRIANGLES
		|| (egl_immediate_size+12) >= EGL_IMMEDIATE_SIZE )	FlushRender();
	/*
			*-*
			|/|
			*-*   Don't forget to go clockwise!
	*/
	egl_immediate_geo_ptr[0] = x1;
	egl_immediate_geo_ptr[1] = y1;
	egl_immediate_geo_ptr[2] = x2;
	egl_immediate_geo_ptr[3] = y1;
	egl_immediate_geo_ptr[4] = x1;
	egl_immediate_geo_ptr[5] = y2;
	egl_immediate_geo_ptr[6] = x1;
	egl_immediate_geo_ptr[7] = y2;
	egl_immediate_geo_ptr[8] = x2;
	egl_immediate_geo_ptr[9] = y1;
	egl_immediate_geo_ptr[10] = x2;
	egl_immediate_geo_ptr[11] = y2;
	egl_immediate_geo_ptr += 12;
	egl_immediate_color_buffer[egl_immediate_size+0] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+1] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+2] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+3] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+4] = egl_currentcolor;
	egl_immediate_color_buffer[egl_immediate_size+5] = egl_currentcolor;
	egl_immediate_size+=6;
	egl_immediate_draw_mode = GL_TRIANGLES;
	egl_immediate_stride = 2;
}

void CNFGTackPoly( RDPoint * points, int verts )
{
	if( egl_immediate_draw_mode != GL_TRIANGLES
		|| egl_immediate_size+verts*2 >= EGL_IMMEDIATE_SIZE )	FlushRender();

	int i;
	for( i = 0; i < verts; i++ )
	{
		egl_immediate_geo_ptr[0] = points[i].x;
		egl_immediate_geo_ptr[1] = points[i].y;
		egl_immediate_geo_ptr += 2;
		egl_immediate_color_buffer[egl_immediate_size] = egl_currentcolor;
		egl_immediate_size++;
	}
	egl_immediate_draw_mode = GL_TRIANGLES;
	egl_immediate_stride = 2;
}

void CNFGClearFrame()
{
	glClearColor( (CNFGBGColor&0xff)/255.0, 
		(CNFGBGColor&0xff00)/65280.0, 
		(CNFGBGColor&0xff0000)/16711680.0,
		((CNFGBGColor&0xff000000)>>24)/255.0);
	glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);
}

void CNFGSwapBuffers()
{
	FlushRender();
	eglSwapBuffers(egl_display, egl_surface);
#ifdef ANDROID
	android_width = ANativeWindow_getWidth( native_window );
	android_height = ANativeWindow_getHeight( native_window );
	glUniform4f( default_screenscale_offset, 2./android_width, -2./android_height, 1.0, 1.0 );
	glViewport( 0, 0, android_width, android_height );
#endif
}

void CNFGGetDimensions( short * x, short * y )
{
#ifdef ANDROID
	*x = android_width;
	*y = android_height;
#else
	*x = native_window.width;
	*y = native_window.height;
#endif
}

int CNFGSetup( const char * WindowName, int w, int h )
{
	int ret;
	EGLint egl_major, egl_minor;
	EGLConfig config;
	EGLint num_config;
	EGLContext context;
	GLuint program;

#ifdef RASPI_GPU
	bcm_host_init();
#endif

#ifdef USE_EGL_X
	XDisplay = XOpenDisplay(NULL);
	if (!XDisplay) {
		fprintf(stderr, "Error: failed to open X display.\n");
		return -1;
	}

	Window XRoot = DefaultRootWindow(XDisplay);

	XSetWindowAttributes XWinAttr;
	XWinAttr.event_mask  =  ExposureMask | PointerMotionMask;

	XWindow = XCreateWindow(XDisplay, XRoot, 0, 0, WIDTH, HEIGHT, 0,
				CopyFromParent, InputOutput,
				CopyFromParent, CWEventMask, &XWinAttr);

	Atom XWMDeleteMessage =
		XInternAtom(XDisplay, "WM_DELETE_WINDOW", False);

	XMapWindow(XDisplay, XWindow);
	XStoreName(XDisplay, XWindow, "Mali libs test");
	XSetWMProtocols(XDisplay, XWindow, &XWMDeleteMessage, 1);

	egl_display = eglGetDisplay((EGLNativeDisplayType) XDisplay);
#else

#ifndef ANDROID
	if( w >= 1 && h >= 1 )
	{
		native_window.width = w;
		native_window.height =h;
	}
#endif

	egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
	if (egl_display == EGL_NO_DISPLAY) {
		fprintf(stderr, "Error: No display found!\n");
		return -1;
	}

	if (!eglInitialize(egl_display, &egl_major, &egl_minor)) {
		fprintf(stderr, "Error: eglInitialise failed!\n");
		return -1;
	}

	printf("EGL Version: \"%s\"\n",
	       eglQueryString(egl_display, EGL_VERSION));
	printf("EGL Vendor: \"%s\"\n",
	       eglQueryString(egl_display, EGL_VENDOR));
	printf("EGL Extensions: \"%s\"\n",
	       eglQueryString(egl_display, EGL_EXTENSIONS));

	eglChooseConfig(egl_display, config_attribute_list, &config, 1,
			&num_config);
	printf( "Config: %d\n", num_config );


#ifdef RASPI_GPU
	// get an appropriate EGL frame buffer configuration
	int result = eglBindAPI(EGL_OPENGL_ES_API);
	printf( "Bound API: %d\n", result );
#endif

	printf( "Creating Context\n" );
	context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT,
//				NULL );
				context_attribute_list);
	if (context == EGL_NO_CONTEXT) {
		fprintf(stderr, "Error: eglCreateContext failed: 0x%08X\n",
			eglGetError());
		return -1;
	}
	printf( "Context Created %p\n", context );


#ifdef USE_EGL_X
	egl_surface = eglCreateWindowSurface(egl_display, config, XWindow,
					     window_attribute_list);
#else

#ifdef RASPI_GPU
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	int adjust = 0;
	if( w > 2048 )
	{
		adjust = (w - 2048) / 2;
		w = 2048;
		printf( "Adjusting window to: %d\n", adjust );
	}

	dst_rect.x = adjust;
	dst_rect.y = 0;
	dst_rect.width = w;
	dst_rect.height = h;

	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = w << 16;
	src_rect.height = h << 16;

	rpi_state.w = w;
	rpi_state.h = h;
	rpi_state.dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	dispman_update = vc_dispmanx_update_start( 0 );
	rpi_state.dispman_element = vc_dispmanx_element_add ( dispman_update, rpi_state.dispman_display,
	      0/*layer*/, &dst_rect, 0/*src*/,
	      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
	printf( "DISPMAN ELEMENT: %d(%d, %d)\n", rpi_state.dispman_element, dispman_update, rpi_state.dispman_display );
	native_window.element = rpi_state.dispman_element;
	native_window.width = w;
	native_window.height = h;
	vc_dispmanx_update_submit_sync( dispman_update );
#endif

	printf( "Getting Surface %p\n", native_window = gapp->window );
	android_width = ANativeWindow_getWidth( native_window );
	android_height = ANativeWindow_getHeight( native_window );
	printf( "Width/Height: %dx%d\n", android_width, android_height );
	egl_surface = eglCreateWindowSurface(egl_display, config,
#ifdef ANDROID
			     gapp->window,
#else
			     (EGLNativeWindowType)&native_window,
#endif
			     window_attribute_list);
#endif
	printf( "Got Surface: %p\n", egl_surface );

	if (egl_surface == EGL_NO_SURFACE) {
		fprintf(stderr, "Error: eglCreateWindowSurface failed: "
			"0x%08X\n", eglGetError());
		return -1;
	}

#ifndef ANDROID
	int width, height;
	if (!eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &width) ||
	    !eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &height)) {
		fprintf(stderr, "Error: eglQuerySurface failed: 0x%08X\n",
			eglGetError());
		return -1;
	}
	printf("Surface size: %dx%d\n", width, height);

	native_window.width = width;
	native_window.height = height;
#endif

	if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, context)) {
		fprintf(stderr, "Error: eglMakeCurrent() failed: 0x%08X\n",
			eglGetError());
		return -1;
	}

	printf("GL Vendor: \"%s\"\n", glGetString(GL_VENDOR));
	printf("GL Renderer: \"%s\"\n", glGetString(GL_RENDERER));
	printf("GL Version: \"%s\"\n", glGetString(GL_VERSION));
	printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

	default_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	if (!default_vertex_shader) {
		fprintf(stderr, "Error: glCreateShader(GL_VERTEX_SHADER) "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}

	glShaderSource(default_vertex_shader, 1, &default_vertex_shader_source, NULL);
	glCompileShader(default_vertex_shader);

	glGetShaderiv(default_vertex_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf(stderr, "Error: vertex shader compilation failed!\n");
		glGetShaderiv(default_vertex_shader, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetShaderInfoLog(default_vertex_shader, ret, NULL, log);
			fprintf(stderr, "%s", log);
		}
		return -1;
	}

	default_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!default_fragment_shader) {
		fprintf(stderr, "Error: glCreateShader(GL_FRAGMENT_SHADER) "
			"failed: 0x%08X\n", glGetError());
		return -1;
	}

	glShaderSource(default_fragment_shader, 1, &default_fragment_shader_source, NULL);
	glCompileShader(default_fragment_shader);

	glGetShaderiv(default_fragment_shader, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf(stderr, "Error: fragment shader compilation failed!\n");
		glGetShaderiv(default_fragment_shader, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetShaderInfoLog(default_fragment_shader, ret, NULL, log);
			fprintf(stderr, "%s", log);
		}
		return -1;
	}

	program = glCreateProgram();
	if (!program) {
		fprintf(stderr, "Error: failed to create program!\n");
		return -1;
	}

	glAttachShader(program, default_vertex_shader);
	glAttachShader(program, default_fragment_shader);

	glBindAttribLocation(program, 0, "aPosition");
	glBindAttribLocation(program, 1, "aColor");

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &ret);
	if (!ret) {
		char *log;

		fprintf(stderr, "Error: program linking failed!\n");
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			log = malloc(ret);
			glGetProgramInfoLog(program, ret, NULL, log);
			fprintf(stderr, "%s", log);
		}
		return -1;
	}
	glUseProgram(program);
	default_screenscale_offset = glGetUniformLocation ( program , "screenscale" );

	egl_immediate_geo_ptr = &egl_immediate_geo_buffer[0];

	//You can put OpenGL ES Stuff here.
	//glLineWidth(10.0);
	//glEnable(GL_DEPTH_TEST);

	return 0;
}

void CNFGSetupFullscreen( const char * WindowName, int screen_number )
{
	CNFGSetup( WindowName, -1, -1 );
}

int32_t handle_input(struct android_app* app, AInputEvent* event)
{
#ifdef ANDROID
	//Potentially do other things here.

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		static uint64_t downmask;

		int action = AMotionEvent_getAction( event );
		int whichsource = action >> 8;
		action &= AMOTION_EVENT_ACTION_MASK;
		size_t pointerCount = AMotionEvent_getPointerCount(event);

		for (size_t i = 0; i < pointerCount; ++i)
		{
			int x, y, index;
			x = AMotionEvent_getX(event, i);
			y = AMotionEvent_getY(event, i);
			index = AMotionEvent_getPointerId( event, i );

			if( action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN )
			{
				HandleButton( x, y, whichsource, 1 );
				downmask    |= 1<<whichsource;
				ANativeActivity_showSoftInput( gapp->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED );
			}
			else if( action == AMOTION_EVENT_ACTION_POINTER_UP || action == AMOTION_EVENT_ACTION_UP )
			{
				HandleButton( x, y, whichsource, 0 );
				downmask    &= ~(1<<whichsource);
			}
			else if( action == AMOTION_EVENT_ACTION_MOVE )
			{
				HandleMotion( x, y, downmask );
			}
		}
		return 1;
	}
	else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
#ifdef ANDROID_USE_SCANCODES
		HandleKey( AKeyEvent_getKeyCode(event), AKeyEvent_getAction(event) );
#else
		HandleKey( AndroidGetUnicodeChar( AKeyEvent_getKeyCode(event), AMotionEvent_getMetaState( event ) ), AKeyEvent_getAction(event) );
#endif

		return 1;
	}
#endif
	return 0;
}

void CNFGHandleInput()
{

#ifdef ANDROID
	int events;
	struct android_poll_source* source;
	while( ALooper_pollAll( 0, 0, &events, (void**)&source) >= 0 )
	{
		if (source != NULL)
		{
			source->process(gapp, source);
		}
	}
#endif

#ifdef USE_EGL_X
	while (1) {
		XEvent event;

		XNextEvent(XDisplay, &event);

		if ((event.type == MotionNotify) ||
		    (event.type == Expose))
			Redraw(width, height);
		else if (event.type == ClientMessage) {
			if (event.xclient.data.l[0] == XWMDeleteMessage)
				break;
		}
	}
	XSetWMProtocols(XDisplay, XWindow, &XWMDeleteMessage, 0);
#endif
}



#ifdef ANDROID

static int OGLESStarted;

void handle_cmd(struct android_app* app, int32_t cmd)
{
	switch (cmd)
	{
	case APP_CMD_INIT_WINDOW:
		if( !OGLESStarted )
		{
			OGLESStarted = 1;
			printf( "Got start event\n" );
		}
		else
		{
			CNFGSetup( "", -1, -1 );
			HandleResume();
		}
		break;
	case APP_CMD_TERM_WINDOW:
		HandleSuspend();
		//HandleDestroy();
		//exit( 0 );
		break;
	default:
		printf( "event not handled: %d", cmd);
	}
}

void android_main(struct android_app* app)
{
	int events;
	int main( int argc, char ** argv );
	char * argv[] = { "main", 0 };

	gapp = app;
	app->onAppCmd = handle_cmd;
	app->onInputEvent = handle_input;
	printf( "Starting" );

#ifdef ANDROID_FULLSCREEN
	void MakeAndroidFullscreen();
	MakeAndroidFullscreen();
#endif

	while( !OGLESStarted )
	{
		struct android_poll_source* source;
		if (ALooper_pollAll( 0, 0, &events, (void**)&source) >= 0)
		{
			if (source != NULL) source->process(gapp, source);
		}
	}
	printf( "Starting Main\n" );
	main( 1, argv );
	printf( "Main Complete\n" );
}

void MakeAndroidFullscreen()
{
	//Partially based on https://stackoverflow.com/questions/47507714/how-do-i-enable-full-screen-immersive-mode-for-a-native-activity-ndk-app
	const struct JNINativeInterface * env = 0;
	const struct JNINativeInterface ** envptr = &env;
	const struct JNIInvokeInterface ** jniiptr = gapp->activity->vm;
	const struct JNIInvokeInterface * jnii = *jniiptr;

	jnii->AttachCurrentThread( jniiptr, &envptr, NULL);
	env = (*envptr);

	//Get android.app.NativeActivity, then get getWindow method handle, returns view.Window type
	jclass activityClass = env->FindClass( envptr, "android/app/NativeActivity");
	jmethodID getWindow = env->GetMethodID( envptr, activityClass, "getWindow", "()Landroid/view/Window;");
	jobject window = env->CallObjectMethod( envptr, gapp->activity->clazz, getWindow);

	//Get android.view.Window class, then get getDecorView method handle, returns view.View type
	jclass windowClass = env->FindClass( envptr, "android/view/Window");
	jmethodID getDecorView = env->GetMethodID( envptr, windowClass, "getDecorView", "()Landroid/view/View;");
	jobject decorView = env->CallObjectMethod( envptr, window, getDecorView);

	//Get the flag values associated with systemuivisibility
	jclass viewClass = env->FindClass( envptr, "android/view/View");
	const int flagLayoutHideNavigation = env->GetStaticIntField( envptr, viewClass, env->GetStaticFieldID( envptr, viewClass, "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION", "I"));
	const int flagLayoutFullscreen = env->GetStaticIntField( envptr, viewClass, env->GetStaticFieldID( envptr, viewClass, "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN", "I"));
	const int flagLowProfile = env->GetStaticIntField( envptr, viewClass, env->GetStaticFieldID( envptr, viewClass, "SYSTEM_UI_FLAG_LOW_PROFILE", "I"));
	const int flagHideNavigation = env->GetStaticIntField( envptr, viewClass, env->GetStaticFieldID( envptr, viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I"));
	const int flagFullscreen = env->GetStaticIntField( envptr, viewClass, env->GetStaticFieldID( envptr, viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I"));
	const int flagImmersiveSticky = env->GetStaticIntField( envptr, viewClass, env->GetStaticFieldID( envptr, viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I"));

	jmethodID setSystemUiVisibility = env->GetMethodID( envptr, viewClass, "setSystemUiVisibility", "(I)V");

	//Call the decorView.setSystemUiVisibility(FLAGS)
	env->CallVoidMethod( envptr, decorView, setSystemUiVisibility,
		        (flagLayoutHideNavigation | flagLayoutFullscreen | flagLowProfile | flagHideNavigation | flagFullscreen | flagImmersiveSticky));

	//now set some more flags associated with layoutmanager -- note the $ in the class path
	//search for api-versions.xml
	//https://android.googlesource.com/platform/development/+/refs/tags/android-9.0.0_r48/sdk/api-versions.xml

	jclass layoutManagerClass = env->FindClass( envptr, "android/view/WindowManager$LayoutParams");
	const int flag_WinMan_Fullscreen = env->GetStaticIntField( envptr, layoutManagerClass, (env->GetStaticFieldID( envptr, layoutManagerClass, "FLAG_FULLSCREEN", "I") ));
	const int flag_WinMan_KeepScreenOn = env->GetStaticIntField( envptr, layoutManagerClass, (env->GetStaticFieldID( envptr, layoutManagerClass, "FLAG_KEEP_SCREEN_ON", "I") ));
	const int flag_WinMan_hw_acc = env->GetStaticIntField( envptr, layoutManagerClass, (env->GetStaticFieldID( envptr, layoutManagerClass, "FLAG_HARDWARE_ACCELERATED", "I") ));
	//    const int flag_WinMan_flag_not_fullscreen = env->GetStaticIntField(layoutManagerClass, (env->GetStaticFieldID(layoutManagerClass, "FLAG_FORCE_NOT_FULLSCREEN", "I") ));
	//call window.addFlags(FLAGS)
	env->CallVoidMethod( envptr, window, (env->GetMethodID (envptr, windowClass, "addFlags" , "(I)V")), (flag_WinMan_Fullscreen | flag_WinMan_KeepScreenOn | flag_WinMan_hw_acc));

	jnii->DetachCurrentThread( jniiptr );
}


void AndroidDisplayKeyboard(int pShow)
{
	//Based on https://stackoverflow.com/questions/5864790/how-to-show-the-soft-keyboard-on-native-activity
	jint lFlags = 0;
	const struct JNINativeInterface * env = 0;
	const struct JNINativeInterface ** envptr = &env;
	const struct JNIInvokeInterface ** jniiptr = gapp->activity->vm;
	const struct JNIInvokeInterface * jnii = *jniiptr;

	jnii->AttachCurrentThread( jniiptr, &envptr, NULL);
	env = (*envptr);
	jclass activityClass = env->FindClass( envptr, "android/app/NativeActivity");

	// Retrieves NativeActivity.
	jobject lNativeActivity = gapp->activity->clazz;


	// Retrieves Context.INPUT_METHOD_SERVICE.
	jclass ClassContext = env->FindClass( envptr, "android/content/Context");
	jfieldID FieldINPUT_METHOD_SERVICE = env->GetStaticFieldID( envptr, ClassContext, "INPUT_METHOD_SERVICE", "Ljava/lang/String;" );
	jobject INPUT_METHOD_SERVICE = env->GetStaticObjectField( envptr, ClassContext, FieldINPUT_METHOD_SERVICE );

	// Runs getSystemService(Context.INPUT_METHOD_SERVICE).
	jclass ClassInputMethodManager = env->FindClass( envptr, "android/view/inputmethod/InputMethodManager" );
	jmethodID MethodGetSystemService = env->GetMethodID( envptr, activityClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	jobject lInputMethodManager = env->CallObjectMethod( envptr, lNativeActivity, MethodGetSystemService, INPUT_METHOD_SERVICE);

	// Runs getWindow().getDecorView().
	jmethodID MethodGetWindow = env->GetMethodID( envptr, activityClass, "getWindow", "()Landroid/view/Window;");
	jobject lWindow = env->CallObjectMethod( envptr, lNativeActivity, MethodGetWindow);
	jclass ClassWindow = env->FindClass( envptr, "android/view/Window");
	jmethodID MethodGetDecorView = env->GetMethodID( envptr, ClassWindow, "getDecorView", "()Landroid/view/View;");
	jobject lDecorView = env->CallObjectMethod( envptr, lWindow, MethodGetDecorView);

	if (pShow) {
		// Runs lInputMethodManager.showSoftInput(...).
		jmethodID MethodShowSoftInput = env->GetMethodID( envptr, ClassInputMethodManager, "showSoftInput", "(Landroid/view/View;I)Z");
		jboolean lResult = env->CallBooleanMethod( envptr, lInputMethodManager, MethodShowSoftInput, lDecorView, lFlags);
	} else {
		// Runs lWindow.getViewToken()
		jclass ClassView = env->FindClass( envptr, "android/view/View");
		jmethodID MethodGetWindowToken = env->GetMethodID( envptr, ClassView, "getWindowToken", "()Landroid/os/IBinder;");
		jobject lBinder = env->CallObjectMethod( envptr, lDecorView, MethodGetWindowToken);

		// lInputMethodManager.hideSoftInput(...).
		jmethodID MethodHideSoftInput = env->GetMethodID( envptr, ClassInputMethodManager, "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");
		jboolean lRes = env->CallBooleanMethod( envptr, lInputMethodManager, MethodHideSoftInput, lBinder, lFlags);
	}

	// Finished with the JVM.
	jnii->DetachCurrentThread( jniiptr );
}

int AndroidGetUnicodeChar( int keyCode, int metaState )
{
	//https://stackoverflow.com/questions/21124051/receive-complete-android-unicode-input-in-c-c/43871301

	int eventType = AKEY_EVENT_ACTION_DOWN;
	const struct JNINativeInterface * env = 0;
	const struct JNINativeInterface ** envptr = &env;
	const struct JNIInvokeInterface ** jniiptr = gapp->activity->vm;
	const struct JNIInvokeInterface * jnii = *jniiptr;

	jnii->AttachCurrentThread( jniiptr, &envptr, NULL);
	env = (*envptr);
	jclass activityClass = env->FindClass( envptr, "android/app/NativeActivity");

	// Retrieves NativeActivity.
	jobject lNativeActivity = gapp->activity->clazz;

	jclass class_key_event = env->FindClass( envptr, "android/view/KeyEvent");
	int unicodeKey;

	jmethodID method_get_unicode_char = env->GetMethodID( envptr, class_key_event, "getUnicodeChar", "(I)I");
	jmethodID eventConstructor = env->GetMethodID( envptr, class_key_event, "<init>", "(II)V");
	jobject eventObj = env->NewObject( envptr, class_key_event, eventConstructor, eventType, keyCode);

	unicodeKey = env->CallIntMethod( envptr, eventObj, method_get_unicode_char, metaState );

	// Finished with the JVM.
	jnii->DetachCurrentThread( jniiptr );

	printf("Unicode key is: %d", unicodeKey);
	return unicodeKey;
}
#endif

