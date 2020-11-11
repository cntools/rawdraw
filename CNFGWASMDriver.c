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
void FastPipeGeometry( short * fv, uint8_t * col, int vertcount );
float sqrtf( float f );


//Geometry batching system - so we can batch geometry and deliver it all at once.
#define VERTCOUNT 8192 //98,304 bytes.
short vertdataV[VERTCOUNT*2];
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

void EmitQuad( short cx0, short cy0, short cx1, short cy1, short cx2, short cy2, short cx3, short cy3 ) 
{
	//Because quads are really useful, but it's best to keep them all triangles if possible.
	if( vertplace >= VERTCOUNT-6 ) FastPipeEmit();
	short * fv = &vertdataV[vertplace*2];
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
	x1++; y1++;
	const short l2 = wgl_last_width_over_2;
	EmitQuad( x1-l2, y1-l2, x1+l2, y1-l2, x1-l2, y1+l2, x1+l2, y1+l2 );
}

void print( double idebug );


void CNFGTackSegment( short x1, short y1, short x2, short y2 )
{
	float ix1 = x1;
	float iy1 = y1;
	float ix2 = x2;
	float iy2 = y2;

	float dx = ix2-ix1;
	float dy = iy2-iy1;
	float imag = 1./sqrtf(dx*dx+dy*dy);
	dx *= imag;
	dy *= imag;
	float orthox = dy*wgl_last_width_over_2;
	float orthoy =-dx*wgl_last_width_over_2;

	ix2 += dx/2 + 1;
	iy2 += dy/2 + 1;
	ix1 -= dx/2 - 1;
	iy1 -= dy/2 - 1;

	//This logic is incorrect. XXX FIXME.
	EmitQuad( (ix1 - orthox), (iy1 - orthoy), (ix1 + orthox), (iy1 + orthoy), (ix2 - orthox), (iy2 - orthoy), ( ix2 + orthox), ( iy2 + orthoy) );
}

void CNFGTackRectangle( short x1, short y1, short x2, short y2 )
{
	float ix1 = x1;
	float iy1 = y1;
	float ix2 = x2;
	float iy2 = y2;
	EmitQuad( ix1,iy1,ix2,iy1,ix1,iy2,ix2,iy2 );
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
		short * fv = &vertdataV[vertplace*2];
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
	wgl_last_width_over_2 = width/2.0;// + 0.5;
}

void CNFGHandleInput()
{
	//Do nothing.
	//Input is handled on swap frame.
}

#endif
