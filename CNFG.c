//Include this file to get all of rawdraw.  You usually will not
//want to include this in your build, but instead, #include "CNFG.h"
//after #define CNFG_IMPLEMENTATION in one of your C files.

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)
#include "CNFGWinDriver.c"
#elif defined( EGL_LEAN_AND_MEAN )
#include "CNFGEGLLeanAndMean.c"
#elif defined( __android__ )
#include "CNFGEGLDriver.c"
#elif defined( OSX )
#include "CNFGCocoaCGDriver.m"
#else
#include "CNFGXDriver.c"
#endif
#include "CNFGFunctions.c"

