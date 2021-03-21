// Copyright 2010-2021 <>< CNLohr, et. al. (Several other authors, many but not all mentioned)
//	Licensed under the MIT/x11 or NewBSD License you choose.
//
//	CN Foundational Graphics Main Header File.  This is the main header you
//	should include.  See README.md for more details.


#ifndef _CNFG_H
#define _CNFG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Rawdraw flags:
	CNFG3D -> Enable the weird 3D functionality that rawdraw has to allow you to
		write apps which emit basic rawdraw primitives but look 3D!
		CNFG_USE_DOUBLE_FUNCTIONS -> Use double-precision floating point for CNFG3D.
	CNFGOGL -> Use an OpenGL Backend for all rawdraw functionality.
		->Caveat->If using CNFG_HAS_XSHAPE, then, we do something realy wacky.
	CNFGRASTERIZER -> Software-rasterize the rawdraw calls, and, use
		CNFGUpdateScreenWithBitmap to send video to webpage.
	CNFGCONTEXTONLY -> Don't add any drawing functions, only opening a window to
		get an OpenGL context.
		
Usually tested combinations:
 * TCC On Windows and X11 (Linux) with:
    - CNFGOGL on or CNFGOGL off.  If CNFGOGL is off you can use
			CNFG_WINDOWS_DISABLE_BATCH to disable all batching.
		-or-
	- CNFGRASTERIZER

	NOTE: Sometimes you can also use CNFGOGL + CNFGRASTERIZER

 * WASM driver supports both: CNFGRASTERIZER and without CNFGRASTERIZER (Recommended turn rasterizer off)
 * ANDROID (But this automatically sets CNFGRASTERIZER OFF and CNFGOGL ON)
*/


#include <stdint.h>

//Some per-platform logic.
#if defined( ANDROID ) || defined( __android__ )
	#define CNFGOGL
#endif

#if ( defined( CNFGOGL ) || defined( __wasm__ ) ) && !defined(CNFG_HAS_XSHAPE)

	#define CNFG_BATCH 8192 //131,072 bytes.

	#if defined( ANDROID ) || defined( __android__ ) || defined( __wasm__ ) || defined( EGL_LEAN_AND_MEAN )
		#define CNFGEWGL //EGL or WebGL
	#else
		#define CNFGDESKTOPGL
	#endif
#endif

typedef struct {
    short x, y; 
} RDPoint; 

extern int CNFGPenX, CNFGPenY;
extern uint32_t CNFGBGColor;
extern uint32_t CNFGLastColor;
extern uint32_t CNFGDialogColor; //Only used for DrawBox

//Draws text at CNFGPenX, CNFGPenY, with scale of `scale`.
void CNFGDrawText( const char * text, short scale );

//Determine how large a given test would be to draw.
void CNFGGetTextExtents( const char * text, int * w, int * h, int textsize  );

//Draws a box, outline as whatever the last CNFGColor was set to but also draws
//a rectangle as a background as whatever CNFGDialogColor is set to.
void CNFGDrawBox( short x1, short y1, short x2, short y2 );

//To be provided by driver. Rawdraw uses colors in the format 0xRRGGBBAA
//Note that some backends do not support alpha of any kind.
//Some platforms also support alpha blending.  So, be sure to set alpha to 0xFF
uint32_t CNFGColor( uint32_t RGBA );

//This both updates the screen, and flips, all as a single operation.
void CNFGUpdateScreenWithBitmap( uint32_t * data, int w, int h );

//This is only supported on a FEW architectures, but allows arbitrary
//image blitting.  Note that the alpha channel behavior is different
//on different systems.
void CNFGBlitImage( uint32_t * data, int x, int y, int w, int h );

void CNFGTackPixel( short x1, short y1 );
void CNFGTackSegment( short x1, short y1, short x2, short y2 );
void CNFGTackRectangle( short x1, short y1, short x2, short y2 );
void CNFGTackPoly( RDPoint * points, int verts );
void CNFGClearFrame();
void CNFGSwapBuffers();

void CNFGGetDimensions( short * x, short * y );


//This will setup a window.  Note that w and h have special meaning. On Windows
//and X11, for instance if you set w and h to be negative, then rawdraw will not
//show the window to the user.  This is useful if you just need it for some
//off-screen-rendering purpose.
//
//Return value of 0 indicates success.  Nonzero indicates error.
int CNFGSetup( const char * WindowName, int w, int h ); 

void CNFGSetupFullscreen( const char * WindowName, int screen_number );
void CNFGHandleInput();


//You must provide:
void HandleKey( int keycode, int bDown );
void HandleButton( int x, int y, int button, int bDown );
void HandleMotion( int x, int y, int mask );
void HandleDestroy();

//Internal function for resizing rasterizer for rasterizer-mode.
void CNFGInternalResize( short x, short y ); //don't call this.

//Not available on all systems.  Use The OGL portion with care.
#ifdef CNFGOGL
void   CNFGSetVSync( int vson );
void * CNFGGetExtension( const char * extname );
#endif

//Also not available on all systems.  Transparency.
void	CNFGPrepareForTransparency();
void	CNFGDrawToTransparencyMode( int transp );
void	CNFGClearTransparencyLevel();

//Only available on systems that support it.
void	CNFGSetLineWidth( short width );
void	CNFGChangeWindowTitle( const char * windowtitle );
void	CNFGSetWindowIconData( int w, int h, uint32_t * data );
int 	CNFGSetupWMClass( const char * WindowName, int w, int h , char * wm_res_name_ , char * wm_res_class_ );

//If you're using a batching renderer, for instance on Android or an OpenGL
//You will need to call this function inbetewen swtiching properties of drawing.  This is usually
//only needed if you calling OpenGL / OGLES functions directly and outside of CNFG.
//
//Note that these are the functions that are used on the backends which support this
//sort of thing.
#ifdef CNFG_BATCH

//If you are not using the CNFGOGL driver, you will need to define these in your driver.
void	CNFGEmitBackendTriangles( const float * vertices, const uint32_t * colors, int num_vertices );
void	CNFGBlitImage( uint32_t * data, int x, int y, int w, int h );

//These need to be defined for the specific driver.  
void 	CNFGClearFrame();
void 	CNFGSwapBuffers();

void 	CNFGFlushRender(); //Emit any geometry (lines, squares, polys) which are slated to be rendered.
void	CNFGInternalResize( short x, short y ); //Driver calls this after resize happens.
void	CNFGSetupBatchInternal(); //Driver calls this after setup is complete.

//Useful function for emitting a non-axis-aligned quad.
void 	CNFGEmitQuad( float cx0, float cy0, float cx1, float cy1, float cx2, float cy2, float cx3, float cy3 );

extern int 	CNFGVertPlace;
extern float CNFGVertDataV[CNFG_BATCH*3];
extern uint32_t CNFGVertDataC[CNFG_BATCH];
#endif


#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

#define CNFG_KEY_SHIFT 0x10
#define CNFG_KEY_BACKSPACE 0x08
#define CNFG_KEY_DELETE 0x2E
#define CNFG_KEY_LEFT_ARROW 0x25
#define CNFG_KEY_RIGHT_ARROW 0x27
#define CNFG_KEY_TOP_ARROW 0x26
#define CNFG_KEY_BOTTOM_ARROW 0x28
#define CNFG_KEY_ESCAPE 0x1B
#define CNFG_KEY_ENTER 0x0D

#elif defined( EGL_LEAN_AND_MEAN ) // doesn't have any keys
#elif defined( __android__ ) || defined( ANDROID ) // ^
#elif defined( __wasm__ )

#define CNFG_KEY_SHIFT 16
#define CNFG_KEY_BACKSPACE 8
#define CNFG_KEY_DELETE 46
#define CNFG_KEY_LEFT_ARROW 37
#define CNFG_KEY_RIGHT_ARROW 39
#define CNFG_KEY_TOP_ARROW 38
#define CNFG_KEY_BOTTOM_ARROW 40
#define CNFG_KEY_ESCAPE 27
#define CNFG_KEY_ENTER 13

#else // most likely x11

#define CNFG_KEY_SHIFT 65505
#define CNFG_KEY_BACKSPACE 65288
#define CNFG_KEY_DELETE 65535
#define CNFG_KEY_LEFT_ARROW 65361
#define CNFG_KEY_RIGHT_ARROW 65363
#define CNFG_KEY_TOP_ARROW 65362
#define CNFG_KEY_BOTTOM_ARROW 65364
#define CNFG_KEY_ESCAPE 65307
#define CNFG_KEY_ENTER 65293

#endif

#ifdef CNFG3D

#ifndef __wasm__
#include <math.h>
#endif

#ifdef CNFG_USE_DOUBLE_FUNCTIONS
#define tdCOS cos
#define tdSIN sin
#define tdTAN tan
#define tdSQRT sqrt
#else
#define tdCOS cosf
#define tdSIN sinf
#define tdTAN tanf
#define tdSQRT sqrtf
#endif

#ifdef __wasm__
void tdMATCOPY( float * x, const float * y ); //Copy y into x
#else
#define tdMATCOPY(x,y) memcpy( x, y, 16*sizeof(float))
#endif

#define tdQ_PI 3.141592653589
#define tdDEGRAD (tdQ_PI/180.)
#define tdRADDEG (180./tdQ_PI)


//General Matrix Functions
void tdIdentity( float * f );
void tdZero( float * f );
void tdTranslate( float * f, float x, float y, float z );		//Operates ON f
void tdScale( float * f, float x, float y, float z );			//Operates ON f
void tdRotateAA( float * f, float angle, float x, float y, float z ); 	//Operates ON f
void tdRotateQuat( float * f, float qw, float qx, float qy, float qz ); 	//Operates ON f
void tdRotateEA( float * f, float x, float y, float z );		//Operates ON f
void tdMultiply( float * fin1, float * fin2, float * fout );		//Operates ON f
void tdPrint( const float * f );
void tdTransposeSelf( float * f );

//Specialty Matrix Functions
void tdPerspective( float fovy, float aspect, float zNear, float zFar, float * out ); //Sets, NOT OPERATES. (FOVX=degrees)
void tdLookAt( float * m, float * eye, float * at, float * up );	//Operates ON m
//General point functions
#define tdPSet( f, x, y, z ) { f[0] = x; f[1] = y; f[2] = z; }
void tdPTransform( const float * pin, float * f, float * pout );
void tdVTransform( const float * vin, float * f, float * vout );
void td4Transform( float * kin, float * f, float * kout );
void td4RTransform( float * kin, float * f, float * kout );
void tdNormalizeSelf( float * vin );
void tdCross( float * va, float * vb, float * vout );
float tdDistance( float * va, float * vb );
float tdDot( float * va, float * vb );
#define tdPSub( x, y, z ) { (z)[0] = (x)[0] - (y)[0]; (z)[1] = (x)[1] - (y)[1]; (z)[2] = (x)[2] - (y)[2]; }
#define tdPAdd( x, y, z ) { (z)[0] = (x)[0] + (y)[0]; (z)[1] = (x)[1] + (y)[1]; (z)[2] = (x)[2] + (y)[2]; }

//Stack Functionality
#define tdMATRIXMAXDEPTH 32
extern float * gSMatrix;
void tdPush();
void tdPop();
void tdMode( int mode );
#define tdMODELVIEW 0
#define tdPROJECTION 1

//Final stage tools
void tdSetViewport( float leftx, float topy, float rightx, float bottomy, float pixx, float pixy );
void tdFinalPoint( float * pin, float * pout );

float tdNoiseAt( int x, int y );
float tdFLerp( float a, float b, float t );
float tdPerlin2D( float x, float y );

#endif

extern const unsigned char RawdrawFontCharData[1405];
extern const unsigned short RawdrawFontCharMap[256];

#ifdef __cplusplus
};
#endif


#if defined( ANDROID ) || defined( __android__ )
#include "CNFGAndroid.h"
#endif

#ifdef CNFG_IMPLEMENTATION
#include "CNFG.c"
#endif

#endif

