#if !defined( _CHEW_H ) || defined( TABLEONLY )
#define _CHEW_H

#ifndef TABLEONLY

#if defined( WINDOWS ) || defined( _WINDOWS ) || defined( WIN32 ) || defined( WIN64 )
#ifdef __TINYC__
#define GLsizeiptr intptr_t
#define GLintptr   intptr_t
#endif
//#include <windows.h>
#include <stdint.h>
#else
//Include OpenGL or something maybe?
#endif
#endif

#if defined( WIN32 ) || defined( WINDOWS )
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#ifdef __cplusplus
#ifndef TABLEONLY
extern "C" {
#endif
#endif

#ifndef TABLEONLY

#ifdef EGL_LEAN_AND_MEAN
#include <GLES/gl.h>
#include <GLES3/gl3.h>
#else
#include <GL/gl.h>
#endif

#ifdef NO_OPENGL_HEADERS
#include "chewtypes.h"
#endif

#ifndef chew_FUN_EXPORT
#define chew_FUN_EXPORT extern
#endif

#define CHEWTYPEDEF( ret, name, retcmd, parameters, ... ) \
typedef ret (STDCALL *name##_t)( __VA_ARGS__ );	\
chew_FUN_EXPORT name##_t	name##fnptr; \
chew_FUN_EXPORT ret name( __VA_ARGS__ );

#define CHEWTYPEDEF2( ret, name, usename, retcmd, parameters, ... ) \
typedef ret (STDCALL *usename##_t)( __VA_ARGS__ );	\
chew_FUN_EXPORT usename##_t	usename##fnptr; \
chew_FUN_EXPORT ret usename( __VA_ARGS__ );

#if defined( __TINYC__ ) && ( defined( WINDOWS ) || defined( _WINDOWS ) || defined( WIN32 ) || defined( WIN64 ) )
#undef GLchar
#define GLchar char
#undef GLDEBUGPROC
#define GLDEBUGPROC void*
#endif

#if defined (TCCINSTANCE)
#undef GLvoid
#define GLvoid void
#undef GLintptr
#define GLintptr int*
#undef GLchar
#define GLchar char
#undef GLsizeiptr
#define GLsizeiptr unsigned int
#endif

void chewInit();
void * chewGetProcAddress( const char *name );

#endif

// Add the things you want here; DO NOT put ; at end of line.

CHEWTYPEDEF( void, glGenVertexArrays, , (n,arrays), GLsizei n, GLuint *arrays ) 
CHEWTYPEDEF( void, glBindVertexArray, , (array), GLuint array )
CHEWTYPEDEF( void, glGenBuffers, , (n,buffers), GLsizei n, GLuint * buffers )
CHEWTYPEDEF( void, glBindBuffer, , (target,buffer), GLenum target, GLuint buffer )
CHEWTYPEDEF( void, glBufferData, , (target,size,data,usage), GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage )
CHEWTYPEDEF( void, glNamedBufferData, , (buffer,size,data,usage) , GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage )
CHEWTYPEDEF( void, glEnableVertexAttribArray, , (index), GLuint index )
CHEWTYPEDEF( void, glDisableVertexAttribArray, , (index), GLuint index )
CHEWTYPEDEF( void, glEnableVertexArrayAttrib, , (vaobj,index), GLuint vaobj, GLuint index )
CHEWTYPEDEF( void, glDisableVertexArrayAttrib, , (vaobj,index), GLuint vaobj, GLuint index )
CHEWTYPEDEF( void, glVertexAttribPointer, , (index,size,type,normalized,stride,pointer), GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer )
CHEWTYPEDEF( void, glVertexAttribIPointer, , (index,size,type,stride,pointer), GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer )
CHEWTYPEDEF( void, glVertexAttribLPointer, , (index,size,type,stride,pointer), GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer )
CHEWTYPEDEF( void, glBindAttribLocation, , (program,index,name), GLuint program, GLuint index, const GLchar *name )

CHEWTYPEDEF( void, glDeleteVertexArrays, , (n,arrays), GLsizei n, const GLuint *arrays )
CHEWTYPEDEF( void, glDeleteBuffers, , (n,buffers), GLsizei n, const GLuint * buffers )
CHEWTYPEDEF( void, glBufferSubData, , (target,offset,size,data), GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data )
CHEWTYPEDEF( void, glNamedBufferSubData, , (buffer,offset,size,data), GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data )

//Already covered in SDL_opengl.h
CHEWTYPEDEF2( void, glActiveTexture, glActiveTextureCHEW, , (texture) , GLenum texture )
CHEWTYPEDEF2( void, glSampleCoverage, glSampleCoverageCHEW, , (value,invert), GLfloat value, GLboolean invert )

#ifndef EGL_LEAN_AND_MEAN
#ifndef TCCINSTANCE
CHEWTYPEDEF( void, glDebugMessageCallback, , (callback,userParam), GLDEBUGPROC callback, const void * userParam )
CHEWTYPEDEF( void, glDebugMessageControl, , (source,type,severity,count,ids,enabled), GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled )
#endif
#endif

CHEWTYPEDEF2( void, glGenerateMipmap, glGenerateMipmapCHEW, , (index), GLuint index )

CHEWTYPEDEF( void, glGenFramebuffers, , (n,framebuffers), GLsizei n, GLuint * framebuffers )
CHEWTYPEDEF( void, glGenRenderbuffers, , (n,renderbuffers), GLsizei n, GLuint * renderbuffers )
CHEWTYPEDEF( void, glBindFramebuffer, , (target,framebuffer), GLenum target, GLuint framebuffer )
CHEWTYPEDEF( void, glBindRenderbuffer, , (target,renderbuffer), GLenum target, GLuint renderbuffer )
CHEWTYPEDEF( void, glRenderbufferStorage, , (target,internalformat,width,height), GLenum target, GLenum internalformat, GLsizei width, GLsizei height )
CHEWTYPEDEF( void, glRenderbufferStorageMultisample, , (target,samples,internalformat,width,height), GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )
CHEWTYPEDEF( void, glNamedRenderbufferStorageMultisample, , (renderbuffer,samples,internalformat,width,height), GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )
CHEWTYPEDEF( void, glFramebufferRenderbuffer, ,(target,attachment,renderbuffertarget,renderbuffer), GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )
CHEWTYPEDEF( void, glTexImage2DMultisample, ,(target,samples,internalformat, width, height, fixedsamplelocations),GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations )
CHEWTYPEDEF( void, glFramebufferTexture2D, ,(target,attachment, textarget, texture, level) , GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
CHEWTYPEDEF( void, glBlitFramebuffer, , (srcX0, srcY0, srcX1, srcY1,dstX0, dstY0,dstX1, dstY1,mask, filter) , GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter )
CHEWTYPEDEF( void, glBlitNamedFramebuffer, , (readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter), GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter )
CHEWTYPEDEF( void, glDeleteFramebuffers, , (n,framebuffers), GLsizei n, const GLuint * framebuffers )
CHEWTYPEDEF( void, glDeleteRenderbuffers, , (n,renderbuffers), GLsizei n, const GLuint * renderbuffers )
CHEWTYPEDEF( GLenum, glCheckFramebufferStatus, return, (target) , GLenum target )
CHEWTYPEDEF( GLenum, glCheckNamedFramebufferStatus, return, (framebuffer,target), GLuint framebuffer, GLenum target )
CHEWTYPEDEF( void, glFramebufferTexture, , (target,attachment,texture,level), GLenum target, GLenum attachment, GLuint texture, GLint level )



CHEWTYPEDEF( GLuint, glCreateProgram, return, () , void )
CHEWTYPEDEF( GLuint, glCreateShader, return, (e), GLenum e )
#ifndef EGL_LEAN_AND_MEAN
CHEWTYPEDEF( void, glShaderSource, , (shader,count,string,length), GLuint shader, GLsizei count, const GLchar **string, const GLint *length )
#endif
CHEWTYPEDEF( void, glCompileShader, ,(shader), GLuint shader )
CHEWTYPEDEF( void, glGetShaderiv, , (shader,pname,params), GLuint shader, GLenum pname, GLint *params )
CHEWTYPEDEF( void, glGetShaderInfoLog , , (shader,maxLength, length, infoLog), GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog )
CHEWTYPEDEF( void, glDeleteProgram, , (program), GLuint program )
CHEWTYPEDEF( void, glDeleteShader, , (shader), GLuint shader )
CHEWTYPEDEF( void, glAttachShader, , (program,shader), GLuint program, GLuint shader )
CHEWTYPEDEF( void, glLinkProgram, , (program), GLuint program )
CHEWTYPEDEF( void, glGetProgramiv, , (program,pname,params), GLuint program, GLenum pname, GLint *params )
CHEWTYPEDEF( void, glUseProgram, , (program), GLuint program )
CHEWTYPEDEF( void, glUniform1f, , (location,v0), GLint location, GLfloat v0 )
CHEWTYPEDEF( void, glUniform2f, , (location,v0,v1), GLint location, GLfloat v0, GLfloat v1 )
CHEWTYPEDEF( void, glUniform3f, , (location,v0,v1,v2), GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )
CHEWTYPEDEF( void, glUniform4f, , (location,v0,v1,v2,v3), GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
CHEWTYPEDEF( void, glUniform1i, , (location,v0), GLint location, GLint v0 )
CHEWTYPEDEF( void, glUniform2i, , (location,v0,v1), GLint location, GLint v0, GLint v1 )
CHEWTYPEDEF( void, glUniform3i, , (location,v0,v1,v2), GLint location, GLint v0, GLint v1, GLint v2 )
CHEWTYPEDEF( void, glUniform4i, , (location,v0,v1,v2,v3), GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
CHEWTYPEDEF( void, glUniform1fv, , (location,count,value), GLint location, GLsizei count, const GLfloat *value )
CHEWTYPEDEF( void, glUniform2fv, , (location,count,value), GLint location, GLsizei count, const GLfloat *value )
CHEWTYPEDEF( void, glUniform3fv, , (location,count,value), GLint location, GLsizei count, const GLfloat *value )
CHEWTYPEDEF( void, glUniform4fv, , (location,count,value), GLint location, GLsizei count, const GLfloat *value )
CHEWTYPEDEF2( void, glUniform4fv, glUniform4fvCHEW, , (location,count,value), GLint location, GLsizei count, const GLfloat *value )
CHEWTYPEDEF( void, glUniform1iv, , (location,count,value), GLint location, GLsizei count, const GLint *value )
CHEWTYPEDEF( void, glUniform2iv, , (location,count,value), GLint location, GLsizei count, const GLint *value )
CHEWTYPEDEF( void, glUniform3iv, , (location,count,value), GLint location, GLsizei count, const GLint *value )
CHEWTYPEDEF( void, glUniform4iv, , (location,count,value), GLint location, GLsizei count, const GLint *value )
CHEWTYPEDEF( void, glUniformMatrix2fv, ,(location,count,transpose,value) , GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
CHEWTYPEDEF( void, glUniformMatrix3fv, ,(location,count,transpose,value) , GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
CHEWTYPEDEF( void, glUniformMatrix4fv, ,(location,count,transpose,value) , GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
CHEWTYPEDEF( void, glGetProgramInfoLog, , (program,maxLength, length, infoLog), GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog )
CHEWTYPEDEF( GLint, glGetUniformLocation, return, (program,name), GLuint program, const GLchar *name )

CHEWTYPEDEF( void *, glMapBuffer, return, (target,access), GLenum target, GLenum access )
CHEWTYPEDEF( void *, glMapNamedBuffer, return, (buffer,access), GLuint buffer, GLenum access )
CHEWTYPEDEF( void *, glMapBufferRange, return, (buffer,offset,length,access), GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access )

CHEWTYPEDEF( GLboolean, glUnmapBuffer, return, (target), GLenum target )

#ifdef __cplusplus
#ifndef TABLEONLY
};
#endif
#endif

#endif // _CHEW_H
