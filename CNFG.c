//Include this file to get all of rawdraw.  You usually will not
//want to include this in your build, but instead, #include "CNFG.h"
//after #define CNFG_IMPLEMENTATION in one of your C files.

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
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

