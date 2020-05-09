# CN's Fundamental Graphics Library

An OS-Agnostic (including No OS AT ALL!) drawing system.  It includes lines,
boxes, linetext, points, bitmaps.  

The drawing method can either be by the OS (i.e. SetPixel in Windows),
rasterized in software (we draw our own lines) or by OpenGL where available.
You can reconfigure it with various `#define`s.  It also makes it really easy
to get an OpenGL window on just about any supported platform.

Somewhere unusual and want to open a window and draw stuff?  Use rawdraw.

Other packages too big, bulky or include junk you don't want?  Use rawdraw.

For embedded platforms, `RASTERIZER` is usually the back end which allows you
to have high level commands on a framebuffer.  It's what was used in ESP
Channel 3.

This is **not** like SDL - some drivers have different quirks, some have
more features implemented than others, and a major goal is not to abstract
very much, but almost to provide an exmaple how to use graphics on a specific
platform.


## Usage in project

You probably want to include CNFG as a submodule.

`git submodule add https://github.com/cntools/rawdraw`

To use CNFG, be sure to do this, or include "CNFG.c" in your project.
```
#define CNFG_IMPLEMENTATION
```

## Example program
A more comprehensive example can be found in `rawdraw.c`, but a basic example
is as follows:

```
//Make it so we don't need to include any other C files in our build.
#define CNFG_IMPLEMENTATION

#include <CNFG.h>

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }
int main()
{
	CNFGSetup( "Example App", 1024, 768 );
	while(1)
	{
		short w, h;
		CNFGClearFrame();
		CNFGHandleInput();
		CNFGGetDimensions( &w, &h );
		CNFGColor( 0xffffff );
		CNFGPenX = 1; CNFGPenY = 1;
		CNFGDrawText( "Hello, World", 2 );
		CNFGSwapBuffers();
	}
}
```

## Building

Windows compile:
```
C:\tcc\tcc rawdraw.c -Irawdraw -lgdi32 -luser32
```

Linux compile:
```
gcc -o rawdraw rawdraw.c -lm -lX11
```

Note, with the STB-style header, you don't need to
`#define CNFG_IMPLEMENTATION` anywhere in your code, and instead can also
compile in `cnfg.c`

## Configurations
Rawdraw is a very disjoint set of configurations

CNFG Configuration options include:
* `CNFG_IMPLEMENTATION` = Include code for implementation.
* `__android__` = build Android port
* `WINDOWS`, `WIN32`, `WIN64` = Windows build
* `CNFG3D` = Include CNFG3D with this rawdraw build.  This provides rasterized graphics functions.
* `RASTERIZER` = Make the underlying graphics engine rasterized functions (software rendering)
* `CNFGOGL` = Make underlying functions use OpenGL if possible.

Platform-Specific
* `HAS_XSHAPE` = Include extra functions for handling on-screen display functionality in X11.

Flags you will probably never want to use:
* `EGL_LEAN_AND_MEAN` = Bare bones EGL Driver


## Extra goodies

### chew.h

You can copy-and-paste `chew.c` and `chew.h` into your program for a single-file compile-only
version of GLEW.  This is WAY BETTER than having to deal with the complicated system they have
set up, but it doesn't include all the functions.  You can easily add OpenGL functions to it.

It provides a very easy-to-use interface for OpenGL Extensions.

### os_generic.h

os_generic is a platform independent way of creating threads, managing TLS, mutices, 
semaphores as well as getting the current time.  It has the following configuration options:

`OSG_NO_IMPLEMENTATION` - Do not include implementation as static functions.
`OSG_PREFIX` - Override function prefix
`OSG_NOSTATIC` - Do not declare the functions as static.

## Warnings

OSX Support is not full-fledged. 
