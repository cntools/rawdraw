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
#elif defined(WINDOWS) || defined(__WINDOWS__) || defined(_WINDOWS) \
                     || defined(WIN32)       || defined(WIN64) \
                     || defined(_WIN32)      || defined(_WIN64) \
                     || defined(__WIN32__)   || defined(__CYGWIN__) \
                     || defined(__MINGW32__) || defined(__MINGW64__) \
                     || defined(__TOS_WIN__)
#include "CNFGWinDriver.c"
#elif defined( EGL_LEAN_AND_MEAN )
#include "CNFGEGLLeanAndMean.c"
#elif defined(__ANDROID__) || defined(__android__) || defined(ANDROID)
#include "CNFGEGLDriver.c"
#else
#include "CNFGXDriver.c"
#endif

#include "CNFGFunctions.c"

#ifdef CNFG3D
#include "CNFG3D.c"
#endif

