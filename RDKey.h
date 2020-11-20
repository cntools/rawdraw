/*
 * Definitions for non-literal keys.
 * TODO: Keycodes for Windows are not defined just yet.
 */

#ifndef _RDKEY_H
#define _RDKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

// TODO
#define CNFG_KEY_SHIFT NULL
#define CNFG_KEY_BACKSPACE NULL
#define CNFG_KEY_DELETE NULL
#define CNFG_KEY_LEFT_ARROW NULL
#define CNFG_KEY_RIGHT_ARROW NULL
#define CNFG_KEY_TOP_ARROW NULL
#define CNFG_KEY_BOTTOM_ARROW NULL
#define CNFG_KEY_ESCAPE NULL

#elif defined( EGL_LEAN_AND_MEAN ) // doesn't have any keys
#elif defined( __android__ ) || defined( ANDROID ) // ^
#elif defined( __wasm__ )

#define CNFG_KEY_SHIFT 16
#define CNFG_KEY_BACKSPACE 8
#define CNFG_KEY_DELETE 46
#define CNFG_KEY_LEFT_ARROW 37
#define CNFG_KEY_RIGHT_ARROW 39
#define CNFG_KEY_TOP_ARROW 38
#define CNFG_KEY_BOTTOM_ARROW 40
#define CNFG_KEY_ESCAPE 27

#else // most likely x11

#define CNFG_KEY_SHIFT 65505
#define CNFG_KEY_BACKSPACE 65288
#define CNFG_KEY_DELETE 65535
#define CNFG_KEY_LEFT_ARROW 65361
#define CNFG_KEY_RIGHT_ARROW 65363
#define CNFG_KEY_TOP_ARROW 65362
#define CNFG_KEY_BOTTOM_ARROW 65364
#define CNFG_KEY_ESCAPE 65307

#endif

#endif
