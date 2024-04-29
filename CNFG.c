//Include this file to get all of rawdraw.  You usually will not
//want to include this in your build, but instead, #include "CNFG.h"
//after #define CNFG_IMPLEMENTATION in one of your C files.

// Defined here for universal definition
int CNFGLastCharacter = 0;
int CNFGLastScancode = 0;

#if defined( CNFGHTTP )
#include "CNFGHTTP.c"
#elif defined( __wasm__ )
#include "CNFGWASMDriver.c"
#elif defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#include "CNFGWinDriver.c"
#elif defined( EGL_LEAN_AND_MEAN )
#include "CNFGEGLLeanAndMean.c"
#elif defined( __android__ ) || defined( ANDROID )
#include "CNFGEGLDriver.c"
#else
#include "CNFGXDriver.c"
#endif

#include "CNFGFunctions.c"

#ifdef CNFG3D
#include "CNFG3D.c"
#endif

