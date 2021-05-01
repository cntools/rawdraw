#ifndef _CHEWTYPES_H
#define _CHEWTYPES_H

#include <stdint.h>

typedef int GLfixed;
typedef int GLclampx;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef uint32_t GLuint32;
typedef int32_t GLint32;
typedef uint32_t GLuint;
typedef char GLchar;
typedef struct __GLsync *GLsync;
typedef intptr_t GLsizeiptr;
typedef intptr_t GLintptr;

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef GLDEBUGPROC
typedef void (APIENTRY *GLDEBUGPROC)(
	GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar *message, const void *userParam);
#endif

#ifndef GL_DEBUG_OUTPUT_SYNCHRONOUS
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242

#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_GEOMETRY_SHADER                0x8DD9
#define GL_STATIC_DRAW                    0x88E4
#define GL_ARRAY_BUFFER                   0x8892
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_FRAMEBUFFER                    0x8D40
#define GL_MULTISAMPLE                    0x809D
#define GL_RENDERBUFFER                   0x8D41
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_PROGRAM_POINT_SIZE             0x8642
#define GL_SAMPLE_COVERAGE                0x80A0
#define GL_TEXTURE_2D_MULTISAMPLE         0x9100
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_TEXTURE_MAX_ANISOTROPY         0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY     0x84FF
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_POINT_SPRITE_OES               0x8861

#define GL_PIXEL_UNPACK_BUFFER            0x88EC
#define GL_STREAM_DRAW                    0x88E0
#define GL_PIXEL_PACK_BUFFER              0x88EB
#define GL_READ_ONLY                      0x88B8
#define GL_BGRA                           0x80E1
#define GL_STREAM_READ                    0x88E1
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46

#define GL_SAMPLE_ALPHA_TO_COVERAGE			0x809E

#define GL_READ_ONLY                      0x88B8
#define GL_WRITE_ONLY                     0x88B9
#define GL_READ_WRITE                     0x88BA
#define GL_BUFFER_ACCESS                  0x88BB
#define GL_BUFFER_MAPPED                  0x88BC
#define GL_BUFFER_MAP_POINTER             0x88BD
#define GL_STREAM_COPY                    0x88E2
#define GL_STATIC_DRAW                    0x88E4
#define GL_STATIC_READ                    0x88E5
#define GL_STATIC_COPY                    0x88E6
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_DYNAMIC_READ                   0x88E9
#define GL_DYNAMIC_COPY                   0x88EA

#define GL_MAP_READ_BIT                   0x0001
#define GL_MAP_WRITE_BIT                  0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT       0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT      0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT         0x0010
#define GL_MAP_UNSYNCHRONIZED_BIT         0x0020

#define GL_VERTEX_PROGRAM_POINT_SIZE      0x8642
#define GL_DEPTH_CLAMP                    0x864F

#define GL_COLOR_ATTACHMENT0_EXT          0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT          0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT          0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT          0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT          0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT          0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT          0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT          0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT          0x8CE8


#define GL_POINT_SPRITE 0x8861
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_R32F 0x822E
#define GL_RG32F 0x8230
#define GL_RGB32F 0x8815
#define GL_RGBA32F 0x8814
#define GL_R8 0x8229
#define GL_RG8 0x822B
#define GL_RG 0x8227

#endif


#endif
