/*
 * Definitions for non-literal keys.
 */

#ifndef _RDKEY_H
#define _RDKEY_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WINDOWS) || defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

#define CNFG_KEY_SHIFT 0x10
#define CNFG_KEY_BACKSPACE 0x08
#define CNFG_KEY_DELETE 0x2E
#define CNFG_KEY_LEFT_ARROW 0x25
#define CNFG_KEY_RIGHT_ARROW 0x27
#define CNFG_KEY_TOP_ARROW 0x26
#define CNFG_KEY_BOTTOM_ARROW 0x28
#define CNFG_KEY_ESCAPE 0x1B
#define CNFG_KEY_ENTER 0x0D

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
#define CNFG_KEY_ENTER 13

#else // most likely x11

#define CNFG_KEY_SHIFT 65505
#define CNFG_KEY_BACKSPACE 65288
#define CNFG_KEY_DELETE 65535
#define CNFG_KEY_LEFT_ARROW 65361
#define CNFG_KEY_RIGHT_ARROW 65363
#define CNFG_KEY_TOP_ARROW 65362
#define CNFG_KEY_BOTTOM_ARROW 65364
#define CNFG_KEY_ESCAPE 65307
#define CNFG_KEY_ENTER 65293

#endif

#endif
