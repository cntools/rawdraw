//Right now, designed for use with https://github.com/cnlohr/rawdrawwasm/
#include <CNFG.h>
#include <stdint.h>

extern void __attribute__((import_module("bynsyncify"))) CNFGSwapBuffersInternal();


#ifdef CNFGRASTERIZER


//XXX TODO: NEED MEMORY ALLOCATOR
extern unsigned char __heap_base;
unsigned int bump_pointer = (unsigned int)&__heap_base;
void* malloc(unsigned long size) {
	unsigned int ptr = bump_pointer;
	bump_pointer += size;
	return (void *)ptr;
}
void free(void* ptr) {  }

#include "CNFGRasterizer.c"

extern void CNFGUpdateScreenWithBitmapInternal( uint32_t * data, int w, int h );
void CNFGUpdateScreenWithBitmap( uint32_t * data, int w, int h )
{
	CNFGBlitImage( data, 0, 0, w, h );
	CNFGSwapBuffersInternal();
}


void	CNFGSetLineWidth( short width )
{
	//Rasterizer does not support line width.
}

void CNFGHandleInput()
{
	//Do nothing.
	//Input is handled on swap frame.
}

#endif





#ifndef CNFGRASTERIZER

//The normal path.

//Forward declarations that we get from either WASM or our javascript code.
void CNFGClearFrameInternal( uint32_t bgcolor );
void FastPipeGeometry( float * fv, uint8_t * col, int vertcount );
float sqrtf( float f );


//Geometry batching system - so we can batch geometry and deliver it all at once.
#define VERTCOUNT 8192 //98,304 bytes.
float vertdataV[VERTCOUNT*2];
uint32_t vertdataC[VERTCOUNT];
int vertplace;
float wgl_last_width_over_2 = .5;
uint32_t last_color;

void FastPipeEmit()
{
	if( !vertplace ) return;
	FastPipeGeometry( vertdataV, (uint8_t*)vertdataC, vertplace );
	vertplace = 0;
}

void CNFGClearFrame()
{
	FastPipeEmit();
	CNFGClearFrameInternal( CNFGBGColor );
}
void CNFGSwapBuffers()
{
	FastPipeEmit();
	CNFGSwapBuffersInternal( );
}

void EmitQuad( float cx0, float cy0, float cx1, float cy1, float cx2, float cy2, float cx3, float cy3 ) 
{
	//Because quads are really useful, but it's best to keep them all triangles if possible.
	if( vertplace >= VERTCOUNT-6 ) FastPipeEmit();
	float * fv = &vertdataV[vertplace*2];
	fv[0] = cx0; fv[1] = cy0;
	fv[2] = cx1; fv[3] = cy1;
	fv[4] = cx2; fv[5] = cy2;
	fv[6] = cx2; fv[7] = cy2;
	fv[8] = cx1; fv[9] = cy1;
	fv[10] = cx3; fv[11] = cy3;
	uint32_t * col = &vertdataC[vertplace];
	uint32_t color = CNFGLastColor;
	col[0] = color; col[1] = color; col[2] = color; col[3] = color; col[4] = color; col[5] = color;
	vertplace += 6;
}

void CNFGTackPixel( short x1, short y1 )
{
	const float l2 = wgl_last_width_over_2;
	EmitQuad( x1-l2+0.5, y1-l2+0.5, x1+l2+0.5, y1-l2+0.5, x1-l2+0.5, y1+l2+0.5, x1+l2+0.5, y1+l2+0.5 );
}

void print( double idebug );


void CNFGTackSegment( short x1, short y1, short x2, short y2 )
{
	float dx = x2-x1;
	float dy = y2-y1;
	float imag = 1./sqrtf(dx*dx+dy*dy);
	float orthox = dy*wgl_last_width_over_2*imag;
	float orthoy =-dx*wgl_last_width_over_2*imag;
	//This logic is incorrect. XXX FIXME.
	EmitQuad( (short)(x1 - orthox+0.5), (short)(y1 - orthoy+0.5), (short)(x1 + orthox+0.5), (short)(y1 + orthoy+0.5), (short)(x2 - orthox+0.5), (short)(y2 - orthoy+0.5), (short)( x2 + orthox +0.5), (short)( y2 + orthoy +0.5) );
}

void CNFGTackRectangle( short x1, short y1, short x2, short y2 )
{
	EmitQuad( x1,y1,x2,y1,x1,y2,x2,y2 );
}

void CNFGTackPoly( RDPoint * points, int verts )
{
	int i;
	int tris = verts-2;
	if( vertplace >= VERTCOUNT-tris*3 ) FastPipeEmit();

	uint32_t color = CNFGLastColor;
	short * ptrsrc =  (short*)points;

	for( i = 0; i < tris; i++ )
	{
		float * fv = &vertdataV[vertplace*2];
		fv[0] = ptrsrc[0];
		fv[1] = ptrsrc[1];
		fv[2] = ptrsrc[2];
		fv[3] = ptrsrc[3];
		fv[4] = ptrsrc[i*2+4];
		fv[5] = ptrsrc[i*2+5];

		uint32_t * col = &vertdataC[vertplace];
		col[0] = color;
		col[1] = color;
		col[2] = color;

		vertplace += 3;
	}
}

uint32_t CNFGColor( uint32_t RGB )
{
	return CNFGLastColor = RGB;
}

void	CNFGSetLineWidth( short width )
{
	wgl_last_width_over_2 = width/2 + 0.5;
}

void CNFGHandleInput()
{
	//Do nothing.
	//Input is handled on swap frame.
}

#endif
