# CN's Fundamental Graphics Library

An OS-Agnostic (including No OS AT ALL!) drawing system.  It includes lines,
boxes, linetext, points, bitmaps.

The drawing method can either be by the OS (i.e. SetPixel in Windows),
rasterized in software (we draw our own lines) or by OpenGL where available.
You can reconfigure it with various `#define`s.  It also makes it really easy
to get an OpenGL window on just about any supported platform.

Somewhere unusual and want to open a window and draw stuff?  Use rawdraw.

Other packages too big, bulky or include junk you don't want?  Use rawdraw.

Want to use OpenGL really quickly?  Use rawdraw.

For embedded platforms, `CNFGRASTERIZER` is usually the back end which allows
you to have high level commands on a framebuffer.  It's what was used in ESP
Channel 3.  You can even play with the rasterizer on every mode except the
Android port by defining it.  You can even be twisted on Windows, Linux and
on WASM (Web) and define it.

This is **not** like SDL - some drivers have different quirks, some have
more features implemented than others, and a major goal is not to abstract
very much, but almost to provide an exmaple how to use graphics on a specific
platform.

## Platform statuses

 * Windows, 100% Support for CNFGOGL and/or CNFGRASTERIZER, Native (GDI32) does not support alpha-blitting, or variable line width.
 * Linux, 100% Support for CNFGOGL and/or CNFGRASTERIZER, Native (X11) does not support alpha-blitting, or variable line thickness
.
 * Android, 100% Support for CNFGOGL (It is forced on) CNFGRASTERIZER not allowed.
 * WASM, 100% Support for CNFGOGL
 * Other EGL (Like Raspberry Pi Raw Metal) Platforms, same as Android, but fixed window size.
 * EGL, CNFGOGL represent OpenGL or OpenGL ES depending on platform support.
 * Vulkan support in progress.
 * CNFG3D Support has been ported with fixed-precision numericals to ESP8266.
 * Mac/OSX Support currently unknown.  Legacy files moved here: https://github.com/cntools/rawdrawtools/tree/main/attic

## Usage in project

You may want to include CNFG as a submodule or as a single-file header.  You can just download rawdraw_sf.h to the path your project is in and `#include` it.

To use it as a single file (rawdraw_sf.h), you can just `wget https://raw.githubusercontent.com/cntools/rawdraw/master/rawdraw_sf.h`

To use it as a submodule you can:

`git submodule add https://github.com/cntools/rawdraw`

paste the command above on your terminal to get a clone of the repository on your system.

note: Incase you get a error saying 
```
fatal: not a git repository (or any parent up to mount point /)
Stopping at filesystem boundary (GIT_DISCOVERY_ACROSS_FILESYSTEM not set).
```
use the command `git init` first.
	
To use CNFG, be sure to do this, or include "CNFG.c" in your project.
```
#define CNFG_IMPLEMENTATION
```


## Usage on Linux
### Prerequisites

Firstly, make sure you have the necessary packages and libraries installed.

To install on **Debian/Ubuntu**:
```
sudo apt-get update
sudo apt-get install xorg-dev libx11-dev libxinerama-dev libxext-dev mesa-common-dev libglu1-mesa-dev
```
To install on **Arch Linux**:
```
sudo pacman -Sy xorg-server-devel libx11 libxinerama libxext mesa glu
```

### Running

To run the sample program in rawdraw:
1. Clone the repository on your system and run `cd rawdraw/`.
2. Run the command `make` (compiles and creates an executable called "simple" from "simple.c").
3. Run the command `./simple` to execute "simple".


## Example program
A more comprehensive example can be found in `rawdraw.c`, but a basic example
is as follows:

```c


//Make it so we don't need to include any other C files in our build.
#define CNFG_IMPLEMENTATION

#include "rawdraw_sf.h"

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
int HandleDestroy() { return 0; }
int main()
{
	CNFGSetup( "Example App", 1024, 768 );
	while(CNFGHandleInput())
	{
		CNFGBGColor = 0x000080ff; //Dark Blue Background

		short w, h;
		CNFGClearFrame();
		CNFGGetDimensions( &w, &h );

		//Change color to white.
		CNFGColor( 0xffffffff ); 

		CNFGPenX = 1; CNFGPenY = 1;
		CNFGDrawText( "Hello, World", 2 );
		//Draw a white pixel at 30, 30 
		CNFGTackPixel( 30, 30 );         

		//Draw a line from 50,50 to 100,50
		CNFGTackSegment( 50, 50, 100, 50 );

		//Dark Red Color Select
		CNFGColor( 0x800000FF ); 

		//Draw 50x50 box starting at 100,50
		CNFGTackRectangle( 100, 50, 150, 100 ); 

		//Bright Purple Select
		CNFGColor( 0x800000FF ); 

		//Draw a triangle
		RDPoint points[3] = { { 30, 36}, {20, 50}, { 40, 50 } };
		CNFGTackPoly( points, 3 );

		//Draw a bunch of random pixels as a blitted image.
		{
			static uint32_t data[64*64];
			int x, y;

			for( y = 0; y < 64; y++ ) for( x = 0; x < 64; x++ )
				data[x+y*64] = 0xff | (rand()<<8);

			CNFGBlitImage( data, 150, 30, 64, 64 );
		}

		//Display the image and wait for time to display next frame.
		CNFGSwapBuffers();		
	}
}
```

## Building
Windows compile:
Build with clang
```
clang simple.c -o simple -Irawdraw -lopengl32 -lgdi32 -luser32 
```

Build with TCC
```
C:\tcc\tcc simple.c -Irawdraw -lopengl32 -lgdi32 -luser32 C:\windows\system32\msvcrt.dll
```

Linux compile:
```
gcc -o simple simple.c -lm -lX11 -lGL
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
* `CNFGOGL` = Make underlying functions use OpenGL if possible.
* `CNFGRASTERIZER` = Make the underlying graphics engine rasterized functions (software rendering)
* `CNFG3D` = Include CNFG3D with this rawdraw build.  This provides *rasterized* graphics functions.  Only use this option with the rasterizer.

Platform-Specific
* `HAS_XSHAPE` = Include extra functions for handling on-screen display functionality in X11.
* `HAS_XINERAMA` = Use X11's Xinerama to handle full-screen more cleanly.

Flags you will probably never want to use:
* `EGL_LEAN_AND_MEAN` = Bare bones EGL Driver


## Extra goodies

### RDUI

RDUI is mainted by Overlisted and contains a lot of extra features for rawdraw. You can check it out here: https://git.overlisted.net/me/rdui/

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

### TextTool/Text.c etc...

There is a sister repository, https://github.com/cntools/rawdrawtools which houses a text tool.

## Warnings

* OSX Support has been moved to attic.
* No support for openGL 1.0. If your hardware doesn't support openGL 2.0 or newer, disable 
openGL support. (If you are on desktop, it probably does, you just need to install updated
drivers.)
