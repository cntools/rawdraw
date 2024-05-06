/*
 * Copyright (c) 2011-2013 Luc Verhaegen <libv@skynet.be>
 * Copyright (c) 2018-2020 <>< Charles Lohr
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


#if defined( __android__ ) && !defined( ANDROID )
#define ANDROID
#endif

//Note: This interface provides the following two things privately.
//you may "extern" them in your code.


#ifdef ANDROID


#include "CNFGAndroid.h"
extern struct android_app * gapp;
static int OGLESStarted;
void (*HandleCustomEventCallback)();
int android_width, android_height;
int override_android_screen_dimensons = 0;
int android_sdk_version;

#include <android_native_app_glue.h>
#include <jni.h>
#include <android/native_activity.h>
#define ERRLOG(...) printf( __VA_ARGS__ );
#else
#define ERRLOG(...) fprintf( stderr, __VA_ARGS__ );
#endif



#include "CNFG.h"

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

#define EGL_ZBITS 16
#define EGL_IMMEDIATE_SIZE 2048

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

#if defined( ANDROID )
EGLNativeWindowType native_window;
#else
struct fbdev_window native_window;
#endif

#endif


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
#if ANDROIDVERSION >= 28
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
#else
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#endif

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
EGLContext egl_context;
EGLConfig  egl_config;

void CNFGSetVSync( int vson )
{
	eglSwapInterval(egl_display, vson);
}

static short iLastInternalW, iLastInternalH;

void CNFGSwapBuffers()
{
	if ( egl_display == EGL_NO_DISPLAY ) return;
	CNFGFlushRender();
	eglSwapBuffers(egl_display, egl_surface);
#ifdef ANDROID
	if( !override_android_screen_dimensons )
	{
		android_width = ANativeWindow_getWidth( native_window );
		android_height = ANativeWindow_getHeight( native_window );
	}
	glViewport( 0, 0, android_width, android_height );
	if( iLastInternalW != android_width || iLastInternalH != android_height )
		CNFGInternalResize( iLastInternalW=android_width, iLastInternalH=android_height );
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
	if( *x != iLastInternalW || *y != iLastInternalH )
		CNFGInternalResize( iLastInternalW=*x, iLastInternalH=*y );
}

int CNFGSetup( const char * WindowName, int w, int h )
{
	EGLint egl_major, egl_minor;
	EGLint num_config;

	//This MUST be called before doing any initialization.
	int events;
	while( !OGLESStarted )
	{
		struct android_poll_source* source;
		if (ALooper_pollOnce( 0, 0, &events, (void**)&source) >= 0)
		{
			if (source != NULL) source->process(gapp, source);
		}
	}


#ifdef USE_EGL_X
	XDisplay = XOpenDisplay(NULL);
	if (!XDisplay) {
		ERRLOG( "Error: failed to open X display.\n");
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
		ERRLOG( "Error: No display found!\n");
		return -1;
	}

	if (!eglInitialize(egl_display, &egl_major, &egl_minor)) {
		ERRLOG( "Error: eglInitialise failed!\n");
		return -1;
	}

	printf("EGL Version: \"%s\"\n",
	       eglQueryString(egl_display, EGL_VERSION));
	printf("EGL Vendor: \"%s\"\n",
	       eglQueryString(egl_display, EGL_VENDOR));
	printf("EGL Extensions: \"%s\"\n",
	       eglQueryString(egl_display, EGL_EXTENSIONS));

	eglChooseConfig(egl_display, config_attribute_list, &egl_config, 1,
			&num_config);
	printf( "Config: %d\n", num_config );

	printf( "Creating Context\n" );
	egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT,
//				NULL );
				context_attribute_list);
	if (egl_context == EGL_NO_CONTEXT) {
		ERRLOG( "Error: eglCreateContext failed: 0x%08X\n",
			eglGetError());
		return -1;
	}
	printf( "Context Created %p\n", egl_context );

#ifdef USE_EGL_X
	egl_surface = eglCreateWindowSurface(egl_display, egl_config, XWindow,
					     window_attribute_list);
#else

	if( native_window && !gapp->window )
	{
		printf( "WARNING: App restarted without a window.  Cannot progress.\n" );
		exit( 0 );
	}

	printf( "Getting Surface %p\n", native_window = gapp->window );

	if( !native_window )
	{
		printf( "FAULT: Cannot get window\n" );
		return -5;
	}

	if( w <= 0 || h <= 0 )
	{
		android_width = ANativeWindow_getWidth( native_window );
		android_height = ANativeWindow_getHeight( native_window );
	}
	else
	{
		override_android_screen_dimensons = 1;
		android_width = w;
		android_height = h;
	}
	printf( "Width/Height: %dx%d\n", android_width, android_height );
	egl_surface = eglCreateWindowSurface(egl_display, egl_config,
#ifdef ANDROID
			     gapp->window,
#else
			     (EGLNativeWindowType)&native_window,
#endif
			     window_attribute_list);
#endif
	printf( "Got Surface: %p\n", egl_surface );

	if (egl_surface == EGL_NO_SURFACE) {
		ERRLOG( "Error: eglCreateWindowSurface failed: "
			"0x%08X\n", eglGetError());
		return -1;
	}

#ifndef ANDROID
	int width, height;
	if (!eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &width) ||
	    !eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &height)) {
		ERRLOG( "Error: eglQuerySurface failed: 0x%08X\n",
			eglGetError());
		return -1;
	}
	printf("Surface size: %dx%d\n", width, height);

	native_window.width = width;
	native_window.height = height;
#endif

	if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context)) {
		ERRLOG( "Error: eglMakeCurrent() failed: 0x%08X\n",
			eglGetError());
		return -1;
	}

	printf("GL Vendor: \"%s\"\n", glGetString(GL_VENDOR));
	printf("GL Renderer: \"%s\"\n", glGetString(GL_RENDERER));
	printf("GL Version: \"%s\"\n", glGetString(GL_VERSION));
	printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

	CNFGSetupBatchInternal();

	{
		short dummyx, dummyy;
		CNFGGetDimensions( &dummyx, &dummyy );
	}

	return 0;
}

void CNFGSetupFullscreen( const char * WindowName, int screen_number )
{
	//Removes decoration, must be called before setup.
	AndroidMakeFullscreen();

	CNFGSetup( WindowName, -1, -1 );
}

int debuga, debugb, debugc;

int32_t handle_input(struct android_app* app, AInputEvent* event)
{
#ifdef ANDROID
	//Potentially do other things here.

	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
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
				int id = index;
				if( action == AMOTION_EVENT_ACTION_POINTER_DOWN && id != whichsource ) continue;
				HandleButton( x, y, id, 1 );
				ANativeActivity_showSoftInput( gapp->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED );
			}
			else if( action == AMOTION_EVENT_ACTION_POINTER_UP || action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_CANCEL )
			{
				int id = index;
				if( action == AMOTION_EVENT_ACTION_POINTER_UP && id != whichsource ) continue;
				HandleButton( x, y, id, 0 );
			}
			else if( action == AMOTION_EVENT_ACTION_MOVE )
			{
				HandleMotion( x, y, index );
			}
		}
		return 1;
	}
	else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
	{
		int code = AKeyEvent_getKeyCode(event);
#ifdef ANDROID_USE_SCANCODES
		HandleKey( code, AKeyEvent_getAction(event) );
#else
		int unicode = AndroidGetUnicodeChar( code, AMotionEvent_getMetaState( event ) );
		if( unicode )
			HandleKey( unicode, AKeyEvent_getAction(event) );
		else
		{
			HandleKey( code, !AKeyEvent_getAction(event) );
			return (code == 4)?1:0; //don't override functionality.
		}
#endif

		return 1;
	}
#endif
	return 0;
}

int CNFGHandleInput()
{

#ifdef ANDROID
	int events;
	struct android_poll_source* source;
	while( ALooper_pollOnce( 0, 0, &events, (void**)&source) >= 0 )
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
	return 1;
}



#ifdef ANDROID

void (*HandleWindowTermination)();

void handle_cmd(struct android_app* app, int32_t cmd)
{
	switch (cmd)
	{
	case APP_CMD_DESTROY:
		//This gets called initially after back.
		HandleDestroy();
		ANativeActivity_finish( gapp->activity );
		break;
	case APP_CMD_INIT_WINDOW:
		//When returning from a back button suspension, this isn't called.
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
		//This gets called initially when you click "back"
		//This also gets called when you are brought into standby.
		//Not sure why - callbacks here seem to break stuff.
		if( egl_display != EGL_NO_DISPLAY ) {
			eglMakeCurrent( egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
			if( egl_context != EGL_NO_CONTEXT ) {
				eglDestroyContext( egl_display, egl_context );
			}
			if( egl_surface != EGL_NO_SURFACE ) {
				eglDestroySurface( egl_display, egl_surface );
			}
			eglTerminate( egl_display );
		}
		egl_context = EGL_NO_CONTEXT;
		egl_surface = EGL_NO_SURFACE;
		egl_display = EGL_NO_DISPLAY;
#ifdef ANDROID_WANT_WINDOW_TERMINATION
		if( HandleWindowTermination ) HandleWindowTermination();
#endif
		break;

	case APP_CMD_PAUSE:
		HandleSuspend();
		break;

	case APP_CMD_RESUME:
		HandleResume();
		break;
	case APP_CMD_CUSTOM_EVENT:
		if( HandleCustomEventCallback ) HandleCustomEventCallback();
		break;
	default:
		printf( "event not handled: %d\n", cmd);
	}
}

int __system_property_get(const char* name, char* value);

void android_main(struct android_app* app)
{
	int main( int argc, char ** argv );
	char mainptr[5] = { 'm', 'a', 'i', 'n', 0 };
	char * argv[] = { mainptr, 0 };

	{
		char sdk_ver_str[92];
		int len = __system_property_get("ro.build.version.sdk", sdk_ver_str);
		if( len <= 0 )
			android_sdk_version = 0;
		else
			android_sdk_version = atoi(sdk_ver_str);
	}

	gapp = app;
	app->onAppCmd = handle_cmd;
	app->onInputEvent = handle_input;
	printf( "Starting with Android SDK Version: %d\n", android_sdk_version );
	main( 1, argv );
	printf( "Main Complete\n" );
}

#ifdef __cplusplus
#define SETUP_FOR_JAVA_CALL \
	JNIEnv * env = 0; \
	JNIEnv ** envptr = &env; \
	JavaVM * jniiptr = gapp->activity->vm; \
	jniiptr->AttachCurrentThread( (JNIEnv**)&env, 0 ); \
	env = (*envptr);
#define ENVCALL
#define JAVA_CALL_DETACH 	jniiptr->DetachCurrentThread();
#else
#define SETUP_FOR_JAVA_CALL \
	const struct JNINativeInterface * env = (struct JNINativeInterface*)gapp->activity->env; \
	const struct JNINativeInterface ** envptr = &env; \
	const struct JNIInvokeInterface ** jniiptr = gapp->activity->vm; \
	const struct JNIInvokeInterface * jnii = *jniiptr; \
	jnii->AttachCurrentThread( jniiptr, &envptr, NULL); \
	env = (*envptr);
#define ENVCALL envptr,
#define JAVA_CALL_DETACH       	jnii->DetachCurrentThread( jniiptr );
#endif

void AndroidMakeFullscreen()
{
	//Partially based on https://stackoverflow.com/questions/47507714/how-do-i-enable-full-screen-immersive-mode-for-a-native-activity-ndk-app
	SETUP_FOR_JAVA_CALL

	//Get android.app.NativeActivity, then get getWindow method handle, returns view.Window type
	jclass activityClass = env->FindClass( ENVCALL "android/app/NativeActivity");
	jmethodID getWindow = env->GetMethodID( ENVCALL activityClass, "getWindow", "()Landroid/view/Window;");
	jobject window = env->CallObjectMethod( ENVCALL gapp->activity->clazz, getWindow);
	jclass windowClass = env->FindClass( ENVCALL "android/view/Window");
	jmethodID getDecorView = env->GetMethodID( ENVCALL windowClass, "getDecorView", "()Landroid/view/View;");
	jobject decorView = env->CallObjectMethod( ENVCALL window, getDecorView);

	/*
		jclass ClassActivity = env->FindClass( ENVCALL "android/app/Activity" );
		const int flag_WindowProp = env->GetStaticIntField( ENVCALL windowClass, env->GetStaticFieldID( ENVCALL windowClass, "FEATURE_NO_TITLE", "I") );
		jmethodID requestWindowFeature = env->GetMethodID( ENVCALL ClassActivity, "requestWindowFeature", "(I)Z" );
		jobject lNativeActivity = gapp->activity->clazz;
		env->CallBooleanMethod( ENVCALL lNativeActivity, requestWindowFeature, flag_WindowProp );
	*/

	//Get the flag values associated with systemuivisibility
	jclass viewClass = env->FindClass( ENVCALL "android/view/View");
	const int flagLayoutHideNavigation = env->GetStaticIntField( ENVCALL viewClass, env->GetStaticFieldID( ENVCALL viewClass, "SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION", "I"));
	const int flagLayoutFullscreen = env->GetStaticIntField( ENVCALL viewClass, env->GetStaticFieldID( ENVCALL viewClass, "SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN", "I"));
	const int flagLowProfile = env->GetStaticIntField( ENVCALL viewClass, env->GetStaticFieldID( ENVCALL viewClass, "SYSTEM_UI_FLAG_LOW_PROFILE", "I"));
	const int flagHideNavigation = env->GetStaticIntField( ENVCALL viewClass, env->GetStaticFieldID( ENVCALL viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I"));
	const int flagFullscreen = env->GetStaticIntField( ENVCALL viewClass, env->GetStaticFieldID( ENVCALL viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I"));
	const int flagImmersiveSticky = env->GetStaticIntField( ENVCALL viewClass, env->GetStaticFieldID( ENVCALL viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I"));
	const int flagLayoutStable = env->GetStaticIntField( ENVCALL viewClass, env->GetStaticFieldID( ENVCALL viewClass, "SYSTEM_UI_FLAG_LAYOUT_STABLE", "I"));
	jmethodID setSystemUiVisibility = env->GetMethodID( ENVCALL viewClass, "setSystemUiVisibility", "(I)V");
	//Call the decorView.setSystemUiVisibility(FLAGS)
	env->CallVoidMethod( ENVCALL decorView, setSystemUiVisibility,
		        (flagLayoutHideNavigation | flagLayoutFullscreen | flagLowProfile | flagHideNavigation | flagFullscreen | flagImmersiveSticky | flagLayoutStable));

	//now set some more flags associated with layoutmanager -- note the $ in the class path
	//search for api-versions.xml
	//https://android.googlesource.com/platform/development/+/refs/tags/android-9.0.0_r48/sdk/api-versions.xml
	jclass layoutManagerClass = env->FindClass( ENVCALL "android/view/WindowManager$LayoutParams");
	const int flag_WinMan_Fullscreen = env->GetStaticIntField( ENVCALL layoutManagerClass, (env->GetStaticFieldID( ENVCALL layoutManagerClass, "FLAG_FULLSCREEN", "I") ));
	const int flag_WinMan_KeepScreenOn = env->GetStaticIntField( ENVCALL layoutManagerClass, (env->GetStaticFieldID( ENVCALL layoutManagerClass, "FLAG_KEEP_SCREEN_ON", "I") ));
	const int flag_WinMan_hw_acc = env->GetStaticIntField( ENVCALL layoutManagerClass, (env->GetStaticFieldID( ENVCALL layoutManagerClass, "FLAG_HARDWARE_ACCELERATED", "I") ));
	const int flag_WinMan_NoLimits = env->GetStaticIntField( ENVCALL layoutManagerClass, (env->GetStaticFieldID( ENVCALL layoutManagerClass, "FLAG_LAYOUT_NO_LIMITS", "I") ));
	//    const int flag_WinMan_flag_not_fullscreen = env->GetStaticIntField(layoutManagerClass, (env->GetStaticFieldID(layoutManagerClass, "FLAG_FORCE_NOT_FULLSCREEN", "I") ));
	//call window.addFlags(FLAGS)
	env->CallVoidMethod( ENVCALL window, (env->GetMethodID (ENVCALL windowClass, "addFlags" , "(I)V")), (flag_WinMan_Fullscreen | flag_WinMan_KeepScreenOn | flag_WinMan_hw_acc | flag_WinMan_NoLimits));



/*
	// Seems to have no impact, and doesn't work with older Android versions.
	jmethodID setDecorFitsSystemWindows = env->GetMethodID( ENVCALL windowClass, "setDecorFitsSystemWindows", "(Z)V");
	env->CallVoidMethod( ENVCALL window, setDecorFitsSystemWindows, JNI_FALSE );

	// "Immersive Mode" (Since Android 11+)
	jmethodID getWindowInsetsController = env->GetMethodID( ENVCALL viewClass, "getWindowInsetsController", "()Landroid/view/WindowInsetsController;" );
	if( getWindowInsetsController )
	{
		//windowInsetsController.hide(Type.systemBars())
		jobject windowInsetsController = env->CallObjectMethod( ENVCALL decorView, getWindowInsetsController );
		jclass windowInsetsControllerClass = env->FindClass( ENVCALL "android/view/WindowInsetsController" );

		jclass windowInsetsTypeClass = env->FindClass( ENVCALL "android/view/WindowInsets/Type" );
		jmethodID typeSystemBars = env->GetStaticMethodID( ENVCALL windowInsetsTypeClass, "systemBars", "()I" );
		int systemBarsType = env->CallStaticIntMethod( ENVCALL windowInsetsTypeClass, typeSystemBars );

		jmethodID hide = env->GetMethodID( ENVCALL windowInsetsControllerClass, "hide", "(I)V" );
		env->CallVoidMethod( ENVCALL windowInsetsController, hide, systemBarsType );
	}
*/

	JAVA_CALL_DETACH
}

void AndroidDisplayKeyboard(int pShow)
{
	//Based on https://stackoverflow.com/questions/5864790/how-to-show-the-soft-keyboard-on-native-activity
	jint lFlags = 0;
	SETUP_FOR_JAVA_CALL

	jclass activityClass = env->FindClass( ENVCALL "android/app/NativeActivity");

	// Retrieves NativeActivity.
	jobject lNativeActivity = gapp->activity->clazz;


	// Retrieves Context.INPUT_METHOD_SERVICE.
	jclass ClassContext = env->FindClass( ENVCALL "android/content/Context");
	jfieldID FieldINPUT_METHOD_SERVICE = env->GetStaticFieldID( ENVCALL ClassContext, "INPUT_METHOD_SERVICE", "Ljava/lang/String;" );
	jobject INPUT_METHOD_SERVICE = env->GetStaticObjectField( ENVCALL ClassContext, FieldINPUT_METHOD_SERVICE );

	// Runs getSystemService(Context.INPUT_METHOD_SERVICE).
	jclass ClassInputMethodManager = env->FindClass( ENVCALL "android/view/inputmethod/InputMethodManager" );
	jmethodID MethodGetSystemService = env->GetMethodID( ENVCALL activityClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	jobject lInputMethodManager = env->CallObjectMethod( ENVCALL lNativeActivity, MethodGetSystemService, INPUT_METHOD_SERVICE);

	// Runs getWindow().getDecorView().
	jmethodID MethodGetWindow = env->GetMethodID( ENVCALL activityClass, "getWindow", "()Landroid/view/Window;");
	jobject lWindow = env->CallObjectMethod( ENVCALL lNativeActivity, MethodGetWindow);
	jclass ClassWindow = env->FindClass( ENVCALL "android/view/Window");
	jmethodID MethodGetDecorView = env->GetMethodID( ENVCALL ClassWindow, "getDecorView", "()Landroid/view/View;");
	jobject lDecorView = env->CallObjectMethod( ENVCALL lWindow, MethodGetDecorView);

	if (pShow) {
		// Runs lInputMethodManager.showSoftInput(...).
		jmethodID MethodShowSoftInput = env->GetMethodID( ENVCALL ClassInputMethodManager, "showSoftInput", "(Landroid/view/View;I)Z");
		/*jboolean lResult = */env->CallBooleanMethod( ENVCALL lInputMethodManager, MethodShowSoftInput, lDecorView, lFlags);
	} else {
		// Runs lWindow.getViewToken()
		jclass ClassView = env->FindClass( ENVCALL "android/view/View");
		jmethodID MethodGetWindowToken = env->GetMethodID( ENVCALL ClassView, "getWindowToken", "()Landroid/os/IBinder;");
		jobject lBinder = env->CallObjectMethod( ENVCALL lDecorView, MethodGetWindowToken);

		// lInputMethodManager.hideSoftInput(...).
		jmethodID MethodHideSoftInput = env->GetMethodID( ENVCALL ClassInputMethodManager, "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");
		/*jboolean lRes = */env->CallBooleanMethod( ENVCALL lInputMethodManager, MethodHideSoftInput, lBinder, lFlags);
	}

	JAVA_CALL_DETACH
}

int AndroidGetUnicodeChar( int keyCode, int metaState )
{
	//https://stackoverflow.com/questions/21124051/receive-complete-android-unicode-input-in-c-c/43871301

	int eventType = AKEY_EVENT_ACTION_DOWN;

	SETUP_FOR_JAVA_CALL

	//jclass activityClass = env->FindClass( envptr, "android/app/NativeActivity");
	// Retrieves NativeActivity.
	//jobject lNativeActivity = gapp->activity->clazz;

	jclass class_key_event = env->FindClass( ENVCALL "android/view/KeyEvent");
	int unicodeKey;

	jmethodID method_get_unicode_char = env->GetMethodID( ENVCALL class_key_event, "getUnicodeChar", "(I)I");
	jmethodID eventConstructor = env->GetMethodID( ENVCALL class_key_event, "<init>", "(II)V");
	jobject eventObj = env->NewObject( ENVCALL class_key_event, eventConstructor, eventType, keyCode);

	unicodeKey = env->CallIntMethod( ENVCALL eventObj, method_get_unicode_char, metaState );

	// Finished with the JVM.
	JAVA_CALL_DETACH

	//printf("Unicode key is: %d", unicodeKey);
	return unicodeKey;
}


//Based on: https://stackoverflow.com/questions/41820039/jstringjni-to-stdstringc-with-utf8-characters
#ifdef __cplusplus
jstring android_permission_name(JNIEnv ** envptr, const char* perm_name)
#else
jstring android_permission_name(const struct JNINativeInterface ** envptr, const char* perm_name)
#endif
{
	// nested class permission in class android.Manifest,
	// hence android 'slash' Manifest 'dollar' permission
#ifdef __cplusplus
	JNIEnv * env = *envptr;
#else
	const struct JNINativeInterface * env = *envptr;
#endif
	jclass ClassManifestpermission = env->FindClass( ENVCALL "android/Manifest$permission");
	jfieldID lid_PERM = env->GetStaticFieldID( ENVCALL ClassManifestpermission, perm_name, "Ljava/lang/String;" );
	jstring ls_PERM = (jstring)(env->GetStaticObjectField( ENVCALL ClassManifestpermission, lid_PERM )); 
	return ls_PERM;
}

/**
 * \brief Tests whether a permission is granted.
 * \param[in] app a pointer to the android app.
 * \param[in] perm_name the name of the permission, e.g.,
 *   "READ_EXTERNAL_STORAGE", "WRITE_EXTERNAL_STORAGE".
 * \retval true if the permission is granted.
 * \retval false otherwise.
 * \note Requires Android API level 23 (Marshmallow, May 2015)
 */
int AndroidHasPermissions( const char* perm_name)
{
	struct android_app* app = gapp;
	SETUP_FOR_JAVA_CALL

	if( android_sdk_version < 23 )
	{
		printf( "Android SDK version %d does not support AndroidHasPermissions\n", android_sdk_version );
		return 1;
	}

	int result = 0;
	jstring ls_PERM = android_permission_name(envptr, perm_name);

	jint PERMISSION_GRANTED = (-1);

	{
		jclass ClassPackageManager = env->FindClass( ENVCALL "android/content/pm/PackageManager" );
		jfieldID lid_PERMISSION_GRANTED = env->GetStaticFieldID( ENVCALL ClassPackageManager, "PERMISSION_GRANTED", "I" );
		PERMISSION_GRANTED = env->GetStaticIntField( ENVCALL ClassPackageManager, lid_PERMISSION_GRANTED );
	}
	{
		jobject activity = app->activity->clazz;
		jclass ClassContext = env->FindClass( ENVCALL "android/content/Context" );
		jmethodID MethodcheckSelfPermission = env->GetMethodID( ENVCALL ClassContext, "checkSelfPermission", "(Ljava/lang/String;)I" );
		jint int_result = env->CallIntMethod( ENVCALL activity, MethodcheckSelfPermission, ls_PERM );
		result = (int_result == PERMISSION_GRANTED);
	}

	JAVA_CALL_DETACH

	return result;
}

/**
 * \brief Query file permissions.
 * \details This opens the system dialog that lets the user
 *  grant (or deny) the permission.
 * \param[in] app a pointer to the android app.
 * \note Requires Android API level 23 (Marshmallow, May 2015)
 */
void AndroidRequestAppPermissions(const char * perm)
{
	if( android_sdk_version < 23 )
	{
		printf( "Android SDK version %d does not support AndroidRequestAppPermissions\n",android_sdk_version );
		return;
	}

	struct android_app* app = gapp;
	SETUP_FOR_JAVA_CALL

	jobject activity = app->activity->clazz;

	jobjectArray perm_array = env->NewObjectArray( ENVCALL 1, env->FindClass( ENVCALL "java/lang/String"), env->NewStringUTF( ENVCALL "" ) );
	env->SetObjectArrayElement( ENVCALL perm_array, 0, android_permission_name(envptr, perm ) );
	jclass ClassActivity = env->FindClass( ENVCALL "android/app/Activity" );

	jmethodID MethodrequestPermissions = env->GetMethodID( ENVCALL ClassActivity, "requestPermissions", "([Ljava/lang/String;I)V" );

	// Last arg (0) is just for the callback (that I do not use)
	env->CallVoidMethod( ENVCALL activity, MethodrequestPermissions, perm_array, 0 );

	JAVA_CALL_DETACH
}

/* Example:
	int hasperm = android_has_permission( "RECORD_AUDIO" );
	if( !hasperm )
	{
		android_request_app_permissions( "RECORD_AUDIO" );
	}
*/

void AndroidSendToBack( int param )
{
	struct android_app* app = gapp;
	SETUP_FOR_JAVA_CALL
	jobject activity = app->activity->clazz;

	//_glfmCallJavaMethodWithArgs(jni, gapp->activity->clazz, "moveTaskToBack", "(Z)Z", Boolean, false);
	jclass ClassActivity = env->FindClass( ENVCALL "android/app/Activity" );
	jmethodID MethodmoveTaskToBack = env->GetMethodID( ENVCALL ClassActivity, "moveTaskToBack", "(Z)Z" );
	env->CallBooleanMethod( ENVCALL activity, MethodmoveTaskToBack, param );
	JAVA_CALL_DETACH
}

#endif

