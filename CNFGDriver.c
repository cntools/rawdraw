#if defined(WINDOWS) || defined(WIN32) || defined(WIN64)
#include "CNFGWinDriver.c"
#elif defined( EGL_LEAN_AND_MEAN )
#include "CNFGEGLLeanAndMean.c"
#elif defined( __android__ )
#include "CNFGEGLDriver.c"
#else
#include "CNFGXDriver.c"
#endif

