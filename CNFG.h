/* Copyright 2010-2020 <>< Charles Lohr and other various authors as attributed.
	Licensed under the MIT/x11 or NewBSD License you choose.

	CN Foundational Graphics Main Header File.  This is the main header you
	should include.  See README.md for more details.
*/


#ifndef _CNFG_H
#define _CNFG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    short x, y; 
} RDPoint; 

extern int CNFGPenX, CNFGPenY;
extern uint32_t CNFGBGColor;
extern uint32_t CNFGLastColor;
extern uint32_t CNFGDialogColor; //background for boxes

void CNFGDrawText( const char * text, short scale );
void CNFGDrawBox( short x1, short y1, short x2, short y2 );
void CNFGGetTextExtents( const char * text, int * w, int * h, int textsize  );
void CNFGDrawTextbox( int x, int y, const char * text, int textsize ); //ignores pen.

//To be provided by driver.
uint32_t CNFGColor( uint32_t RGB );
void CNFGUpdateScreenWithBitmap( uint32_t * data, int w, int h );
void CNFGTackPixel( short x1, short y1 );
void CNFGTackSegment( short x1, short y1, short x2, short y2 );
void CNFGTackRectangle( short x1, short y1, short x2, short y2 );
void CNFGTackPoly( RDPoint * points, int verts );
void CNFGClearFrame();
void CNFGSwapBuffers();

void CNFGGetDimensions( short * x, short * y );
int CNFGSetup( const char * WindowName, int w, int h ); //return 0 if ok.
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

//If you're using a batching renderer, for instance on Android or an OpenGL
//You will need to call this function inbetewen swtiching properties of drawing.  This is usually
//only needed if you calling OpenGL / OGLES functions directly and outside of CNFG.
void 	CNFGFlushRender();


#ifdef CNFG3D

#include <math.h>

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
#define tdMATCOPY(x,y) memcpy( x, y, 16*sizeof(float))
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


#ifdef CNFG_IMPLEMENTATION
#include "CNFG.c"
#endif

#endif

