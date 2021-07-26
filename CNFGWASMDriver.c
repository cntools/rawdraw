//Right now, designed for use with https://github.com/cnlohr/rawdrawwasm/
#include <CNFG.h>
#include <stdint.h>

extern void __attribute__((import_module("bynsyncify"))) CNFGSwapBuffersInternal();
void CNFGBlitImageInternal( uint32_t * data, int x, int y, int w, int h );
void print( double idebug );
void prints( const char* sdebug );


//Forward declarations that we get from either WASM or our javascript code.
void CNFGClearFrameInternal( uint32_t bgcolor );

//The WASM driver handles internal resizing automatically.
#ifndef CNFGRASTERIZER

void	CNFGInternalResize( short x, short y )
{
}

void CNFGFlushRender()
{
	if( !CNFGVertPlace ) return;
	CNFGEmitBackendTriangles( CNFGVertDataV, CNFGVertDataC, CNFGVertPlace );
	CNFGVertPlace = 0;
}
void CNFGClearFrame()
{
	CNFGFlushRender();
	CNFGClearFrameInternal( CNFGBGColor );
}
void CNFGSwapBuffers()
{
	CNFGFlushRender();
	CNFGSwapBuffersInternal( );
}

int CNFGHandleInput()
{
	//Do nothing.
	//Input is handled on swap frame.
	return 1;
}

void CNFGBlitImage( uint32_t * data, int x, int y, int w, int h )
{
	CNFGBlitImageInternal( data, x, y, w, h );
}

#else
	
//Rasterizer - if you want to do this, you will need to enable blitting in the javascript.
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
	CNFGBlitImageInternal( data, 0, 0, w, h );
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
