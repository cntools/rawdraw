//Copyright (c) 2010-2025 <>< Charles Lohr, and several others!
//	Licensed under the MIT/x11 or NewBSD License you choose.
//
// OpenGL Renderer (platform independent portions)

#include "CNFG.h"

#if !defined( CNFG_WASM ) && !defined( CNFGHTTP )
//In WASM, Javascript takes over this functionality.

//Shader compilation errors go to stderr.
#include <stdio.h>

#ifndef GL_VERTEX_SHADER
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LINK_STATUS                    0x8B82
#define GL_TEXTURE_2D                     0x0DE1
#define GL_CLAMP_TO_EDGE                  0x812F
#define LGLchar char
#else
#define LGLchar GLchar
#endif

#ifdef CNFG_WINDOWS
#define CNFGOGL_NEED_EXTENSION
#include <GL/gl.h>
#endif

#if defined(CNFGCONTEXTONLY) || defined(CNFGOGL_NEED_EXTENSION)
#ifdef CNFG_WINDOWS

//From https://www.khronos.org/opengl/wiki/Load_OpenGL_Functions
void * CNFGGetProcAddress(const char *name)
{
	void *p = (void *)wglGetProcAddress(name);
	if(p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1) )
	{
		static HMODULE module;
		if( !module ) module = LoadLibraryA("opengl32.dll");
		p = (void *)GetProcAddress(module, name);
	}
	// We were unable to load the required openGL function 
	if (!p) {
		fprintf(stderr,"[rawdraw][warn]: Unable to load openGL extension \"%s\"\n", name);
	}
	return p;
}

#else
#include <dlfcn.h>


void * CNFGGetProcAddress(const char *name)
{
	//Tricky use RTLD_NEXT first so we don't accidentally link against ourselves.
	void * v1 = dlsym( (void*)((intptr_t)-1) /*RTLD_NEXT = -1*/ /*RTLD_DEFAULT = 0*/, name );
	//printf( "%s = %p\n", name, v1 );
	if( !v1 ) v1 = dlsym( 0, name );
	return v1;
}

#endif
#endif

#ifdef  CNFGOGL_NEED_EXTENSION
// If we are going to be defining our own function pointer call
	#ifdef CNFG_WINDOWS
	// Make sure to use __stdcall on Windows
		#define CHEWTYPEDEF( ret, name, rv, paramcall, ... ) \
			typedef ret (__stdcall *CNFGTYPE##name)( __VA_ARGS__ ); \
			ret (__stdcall *CNFG##name)( __VA_ARGS__ );
	#else
		#define CHEWTYPEDEF( ret, name, rv, paramcall, ... ) \
			typedef ret (*CNFGTYPE##name)( __VA_ARGS__ ); \
			ret (*CNFG##name)( __VA_ARGS__ );
	#endif
#else
//If we are going to be defining the real call
#define CHEWTYPEDEF( ret, name, rv, paramcall, ... ) \
	ret name (__VA_ARGS__);
#endif

#ifndef CNFGCONTEXTONLY

#ifdef __cplusplus
extern "C" {
#endif

int (*MyFunc)( int program, const LGLchar *name );

CHEWTYPEDEF( GLint, glGetUniformLocation, return, (program,name), GLuint program, const LGLchar *name )
CHEWTYPEDEF( void, glEnableVertexAttribArray, , (index), GLuint index )
CHEWTYPEDEF( void, glUseProgram, , (program), GLuint program )
CHEWTYPEDEF( void, glGetProgramInfoLog, , (program,maxLength, length, infoLog), GLuint program, GLsizei maxLength, GLsizei *length, LGLchar *infoLog )
CHEWTYPEDEF( void, glGetProgramiv, , (program,pname,params), GLuint program, GLenum pname, GLint *params )
CHEWTYPEDEF( void, glBindAttribLocation, , (program,index,name), GLuint program, GLuint index, const LGLchar *name )
CHEWTYPEDEF( void, glGetShaderiv, , (shader,pname,params), GLuint shader, GLenum pname, GLint *params )
CHEWTYPEDEF( GLuint, glCreateShader, return, (e), GLenum e )
CHEWTYPEDEF( void, glVertexAttribPointer, , (index,size,type,normalized,stride,pointer), GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer )
CHEWTYPEDEF( void, glShaderSource, , (shader,count,string,length), GLuint shader, GLsizei count, const LGLchar *const*string, const GLint *length )
CHEWTYPEDEF( void, glAttachShader, , (program,shader), GLuint program, GLuint shader )
CHEWTYPEDEF( void, glCompileShader, ,(shader), GLuint shader )
CHEWTYPEDEF( void, glGetShaderInfoLog , , (shader,maxLength, length, infoLog), GLuint shader, GLsizei maxLength, GLsizei *length, LGLchar *infoLog )
CHEWTYPEDEF( GLuint, glCreateProgram, return, () , void )
CHEWTYPEDEF( void, glLinkProgram, , (program), GLuint program )
CHEWTYPEDEF( void, glDeleteShader, , (shader), GLuint shader )
CHEWTYPEDEF( void, glUniform4f, , (location,v0,v1,v2,v3), GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
CHEWTYPEDEF( void, glUniform1i, , (location,i0), GLint location, GLint i0 )
CHEWTYPEDEF( void, glActiveTexture, , (texture), GLenum texture )

#ifndef CNFGOGL_NEED_EXTENSION
#define CNFGglGetUniformLocation glGetUniformLocation
#define CNFGglEnableVertexAttribArray glEnableVertexAttribArray
#define CNFGglUseProgram glUseProgram
#define CNFGglEnableVertexAttribArray glEnableVertexAttribArray
#define CNFGglUseProgram glUseProgram
#define CNFGglGetProgramInfoLog glGetProgramInfoLog
#define CNFGglGetProgramiv glGetProgramiv
#define CNFGglShaderSource glShaderSource
#define CNFGglCreateShader glCreateShader
#define CNFGglAttachShader glAttachShader
#define CNFGglGetShaderiv glGetShaderiv
#define CNFGglCompileShader glCompileShader
#define CNFGglGetShaderInfoLog glGetShaderInfoLog
#define CNFGglCreateProgram glCreateProgram
#define CNFGglLinkProgram glLinkProgram
#define CNFGglDeleteShader glDeleteShader
#define CNFGglUniform4f glUniform4f
#define CNFGglBindAttribLocation glBindAttribLocation
#define CNFGglVertexAttribPointer glVertexAttribPointer
#define CNFGglUniform1i glUniform1i
#define CNFGglActiveTexture glActiveTexture

#endif

#ifdef __cplusplus
};
#endif

#ifdef CNFGOGL_NEED_EXTENSION

// Try and load openGL extension functions required for rawdraw
static void CNFGLoadExtensionsInternal()
{
	CNFGglGetUniformLocation = (CNFGTYPEglGetUniformLocation) CNFGGetProcAddress( "glGetUniformLocation" );
	CNFGglEnableVertexAttribArray = (CNFGTYPEglEnableVertexAttribArray)CNFGGetProcAddress( "glEnableVertexAttribArray" );
	CNFGglUseProgram = (CNFGTYPEglUseProgram)CNFGGetProcAddress( "glUseProgram" );
	CNFGglGetProgramInfoLog = (CNFGTYPEglGetProgramInfoLog)CNFGGetProcAddress( "glGetProgramInfoLog" );
	CNFGglBindAttribLocation = (CNFGTYPEglBindAttribLocation)CNFGGetProcAddress( "glBindAttribLocation" );
	CNFGglGetProgramiv = (CNFGTYPEglGetProgramiv)CNFGGetProcAddress( "glGetProgramiv" );
	CNFGglGetShaderiv = (CNFGTYPEglGetShaderiv)CNFGGetProcAddress( "glGetShaderiv" );
	CNFGglVertexAttribPointer = (CNFGTYPEglVertexAttribPointer)CNFGGetProcAddress( "glVertexAttribPointer" );
	CNFGglCreateShader = (CNFGTYPEglCreateShader)CNFGGetProcAddress( "glCreateShader" );
	CNFGglShaderSource = (CNFGTYPEglShaderSource)CNFGGetProcAddress( "glShaderSource" );
	CNFGglAttachShader = (CNFGTYPEglAttachShader)CNFGGetProcAddress( "glAttachShader" );
	CNFGglCompileShader = (CNFGTYPEglCompileShader)CNFGGetProcAddress( "glCompileShader" );
	CNFGglGetShaderInfoLog = (CNFGTYPEglGetShaderInfoLog)CNFGGetProcAddress( "glGetShaderInfoLog" );
	CNFGglDeleteShader = (CNFGTYPEglDeleteShader)CNFGGetProcAddress( "glDeleteShader" );
	CNFGglLinkProgram = (CNFGTYPEglLinkProgram)CNFGGetProcAddress( "glLinkProgram" );
	CNFGglCreateProgram = (CNFGTYPEglCreateProgram)CNFGGetProcAddress( "glCreateProgram" );
	CNFGglUniform4f = (CNFGTYPEglUniform4f)CNFGGetProcAddress( "glUniform4f" );
	CNFGglUniform1i = (CNFGTYPEglUniform1i)CNFGGetProcAddress( "glUniform1i" );
	CNFGglActiveTexture = (CNFGTYPEglActiveTexture)CNFGGetProcAddress("glActiveTexture");

	// Check if any of these functions didn't get loaded
	uint8_t not_all_functions_loaded = 
		!CNFGglGetUniformLocation  || !CNFGglEnableVertexAttribArray || !CNFGglUseProgram       ||
		!CNFGglGetProgramInfoLog   || !CNFGglBindAttribLocation      || !CNFGglGetProgramiv     ||
		!CNFGglVertexAttribPointer || !CNFGglCreateShader            || !CNFGglShaderSource     ||
		!CNFGglAttachShader        || !CNFGglCompileShader           || !CNFGglGetShaderInfoLog ||
		!CNFGglDeleteShader        || !CNFGglLinkProgram             || !CNFGglCreateProgram    ||
		!CNFGglUniform4f           || !CNFGglUniform1i               || !CNFGglActiveTexture; 
	if (not_all_functions_loaded) {
		fprintf(
			stderr,
			"[rawdraw][err]: Unable to load all openGL extensions required for rawdraw\n"
			"\tPlease update your graphics drivers or unexpected crashes may occur.\n"
		);
	}

	// Give a very stern warning if unable to create or compile shaders
	if (!CNFGglCreateShader || !CNFGglCompileShader) {
		fprintf(
			stderr,
			"[rawdraw][err]: Unable to create or compile shaders, this will cause a fatal error if "
			"openGL is used.\n"
			"\tUpdate your video graphics drivers or switch to software graphics.\n"
		);
	}
}
#else
static void CNFGLoadExtensionsInternal() { }
#endif



GLuint gRDShaderProg = -1;
GLuint gRDBlitProg = -1;
GLuint gRDShaderProgUX = -1;
GLuint gRDBlitProgUX = -1;
GLuint gRDBlitProgUT = -1;
GLuint gRDBlitProgTex = -1;
GLuint gRDLastResizeW;
GLuint gRDLastResizeH;


GLuint CNFGGLInternalLoadShader( const char * vertex_shader, const char * fragment_shader )
{
	GLuint fragment_shader_object = 0;
	GLuint vertex_shader_object = 0;
	GLuint program = 0;
	int ret;

	vertex_shader_object = CNFGglCreateShader(GL_VERTEX_SHADER);
	if (!vertex_shader_object) {
		fprintf( stderr, "Error: glCreateShader(GL_VERTEX_SHADER) "
			"failed: 0x%08X\n", glGetError());
		goto fail;
	}

	CNFGglShaderSource(vertex_shader_object, 1, &vertex_shader, NULL);
	CNFGglCompileShader(vertex_shader_object);

	CNFGglGetShaderiv(vertex_shader_object, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		fprintf( stderr,"Error: vertex shader compilation failed!\n");
		CNFGglGetShaderiv(vertex_shader_object, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			char * log = (char*)alloca(ret+1);
			CNFGglGetShaderInfoLog(vertex_shader_object, ret, NULL, log);
			fprintf( stderr, "%s", log);
		}
		goto fail;
	}

	fragment_shader_object = CNFGglCreateShader(GL_FRAGMENT_SHADER);
	if (!fragment_shader_object) {
		fprintf( stderr, "Error: glCreateShader(GL_FRAGMENT_SHADER) "
			"failed: 0x%08X\n", glGetError());
		goto fail;
	}

	CNFGglShaderSource(fragment_shader_object, 1, &fragment_shader, NULL);
	CNFGglCompileShader(fragment_shader_object);

	CNFGglGetShaderiv(fragment_shader_object, GL_COMPILE_STATUS, &ret);
	if (!ret) {
		fprintf( stderr, "Error: fragment shader compilation failed!\n");
		CNFGglGetShaderiv(fragment_shader_object, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			char * log = (char*)alloca(ret+1);
			CNFGglGetShaderInfoLog(fragment_shader_object, ret, NULL, log);
			fprintf( stderr, "%s", log);
		}
		goto fail;
	}

	program = CNFGglCreateProgram();
	if (!program) {
		fprintf( stderr, "Error: failed to create program!\n");
		goto fail;
	}

	CNFGglAttachShader(program, vertex_shader_object);
	CNFGglAttachShader(program, fragment_shader_object);

	CNFGglBindAttribLocation(program, 0, "a0");
	CNFGglBindAttribLocation(program, 1, "a1");

	CNFGglLinkProgram(program);

	CNFGglGetProgramiv(program, GL_LINK_STATUS, &ret);
	if (!ret) {
		fprintf( stderr, "Error: program linking failed!\n");
		CNFGglGetProgramiv(program, GL_INFO_LOG_LENGTH, &ret);

		if (ret > 1) {
			//TODO: Refactor to remove malloc reliance.
			#ifndef __clang__
			char *log = (char*)alloca(ret);
			CNFGglGetProgramInfoLog(program, ret, NULL, log);
			fprintf( stderr, "%s", log);
			#endif
		}
		goto fail;
	}
	return program;
fail:
	if( !vertex_shader_object ) CNFGglDeleteShader( vertex_shader_object );
	if( !fragment_shader_object ) CNFGglDeleteShader( fragment_shader_object );
	if( !program ) CNFGglDeleteShader( program );
	return -1;
}

#if defined( CNFGEWGL ) && !defined( CNFG_NO_PRECISION )
#define PRECISIONA "lowp"
#define PRECISIONB "mediump"
#else
#define PRECISIONA
#define PRECISIONB
#endif

void CNFGSetupBatchInternal()
{
	short w, h;

	CNFGLoadExtensionsInternal();

	CNFGGetDimensions( &w, &h );

	gRDShaderProg = CNFGGLInternalLoadShader(
		"uniform vec4 xfrm;"
		"attribute vec3 a0;"
		"attribute vec4 a1;"
		"varying " PRECISIONA " vec4 vc;"
		"void main() { gl_Position = vec4( a0.xy*xfrm.xy+xfrm.zw, a0.z, 0.5 ); vc = a1; }",

		"varying " PRECISIONA " vec4 vc;"
		"void main() { gl_FragColor = vec4(vc.abgr); }" 
	);

	CNFGglUseProgram( gRDShaderProg );
	gRDShaderProgUX = CNFGglGetUniformLocation ( gRDShaderProg , "xfrm" );


	gRDBlitProg = CNFGGLInternalLoadShader(
		"uniform vec4 xfrm;"
		"attribute vec3 a0;"
		"attribute vec4 a1;"
		"varying " PRECISIONB " vec2 tc;"
		"void main() { gl_Position = vec4( a0.xy*xfrm.xy+xfrm.zw, a0.z, 0.5 ); tc = a1.xy; }",
		
		"varying " PRECISIONB " vec2 tc;"
		"uniform sampler2D tex;"
		"void main() { gl_FragColor = texture2D(tex,tc)."

#if !defined( CNFGRASTERIZER )
"wzyx"
#else
"wxyz"
#endif
";}" 	);

	CNFGglUseProgram( gRDBlitProg );
	gRDBlitProgUX = CNFGglGetUniformLocation ( gRDBlitProg , "xfrm" );
	gRDBlitProgUT = CNFGglGetUniformLocation ( gRDBlitProg , "tex" );
	glGenTextures( 1, &gRDBlitProgTex );

	CNFGglEnableVertexAttribArray(0);
	CNFGglEnableVertexAttribArray(1);

	glDisable(GL_DEPTH_TEST);
	glDepthMask( GL_FALSE );
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	CNFGVertPlace = 0;
}

#ifndef CNFGRASTERIZER
void CNFGInternalResize(short x, short y)
#else
void CNFGInternalResizeOGLBACKEND(short x, short y)
#endif
{
	glViewport( 0, 0, x, y );
	gRDLastResizeW = x;
	gRDLastResizeH = y;
	if (gRDShaderProg == 0xFFFFFFFF) { return; } // Prevent trying to set uniform if the shader isn't ready yet.
	CNFGglUseProgram( gRDShaderProg );
	CNFGglUniform4f( gRDShaderProgUX, 1.f/x, -1.f/y, -0.5f, 0.5f);
}

void	CNFGEmitBackendTriangles( const float * vertices, const uint32_t * colors, int num_vertices )
{
	CNFGglUseProgram( gRDShaderProg );
	CNFGglUniform4f( gRDShaderProgUX, 1.f/gRDLastResizeW, -1.f/gRDLastResizeH, -0.5f, 0.5f);
	CNFGglVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	CNFGglVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);
	glDrawArrays( GL_TRIANGLES, 0, num_vertices);
}



#ifdef CNFGRASTERIZER
void CNFGBlitImageInternal( uint32_t * data, int x, int y, int w, int h )
#else
void CNFGBlitImage( uint32_t * data, int x, int y, int w, int h )
#endif
{
	glEnable( GL_TEXTURE_2D );
	CNFGglActiveTexture( 0 );
	glBindTexture( GL_TEXTURE_2D, gRDBlitProgTex );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,  GL_RGBA,
		GL_UNSIGNED_BYTE, data );

	CNFGBlitTex( gRDBlitProgTex, x, y, w, h );
}

#ifndef CNFGRASTERIZER

void CNFGClearFrame()
{
	glClearColor( ((CNFGBGColor&0xff000000)>>24)/255.0f, 
		((CNFGBGColor&0xff0000)>>16)/255.0f,
		(CNFGBGColor&0xff00)/65280.0f,
		(CNFGBGColor&0xff)/255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

#endif

// this is here, so people don't have to include opengl
void CNFGDeleteTex( unsigned int tex )
{
	glDeleteTextures(1, &tex);
}

unsigned int CNFGTexImage( uint32_t *data, int w, int h )
{
	GLuint tex;

	glGenTextures(1, &tex);
	glEnable( GL_TEXTURE_2D );
	CNFGglActiveTexture( 0 );
	glBindTexture( GL_TEXTURE_2D, tex );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,  GL_RGBA,
		GL_UNSIGNED_BYTE, data );

	return (unsigned int)tex;
}

void CNFGBlitTex( unsigned int tex, int x, int y, int w, int h )
{
	if( w == 0 || h == 0 ) return;

	CNFGFlushRender();

	CNFGglUseProgram( gRDBlitProg );
	CNFGglUniform4f( gRDBlitProgUX,
		1.f/gRDLastResizeW, -1.f/gRDLastResizeH,
		-0.5f+x/(float)gRDLastResizeW, 0.5f-y/(float)gRDLastResizeH );
	CNFGglUniform1i( gRDBlitProgUT, 0 );

	glBindTexture(GL_TEXTURE_2D, tex);

	const float verts[] = {
		0,0, (float)w,0, (float)w,(float)h,
		0,0, (float)w,(float)h, 0,(float)h, };
	static const uint8_t tex_verts[] = {
		0,0,   255,0,  255,255,
		0,0,  255,255, 0,255 };

	CNFGglVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, verts);
	CNFGglVertexAttribPointer(1, 2, GL_UNSIGNED_BYTE, GL_TRUE, 0, tex_verts);

	glDrawArrays( GL_TRIANGLES, 0, 6);
}

void CNFGSetScissors( int * xywh )
{
	CNFGFlushRender();
	glEnable( GL_SCISSOR_TEST );
	glScissor( xywh[0], xywh[1], xywh[2], xywh[3] );
}

void CNFGGetScissors( int * xywh )
{
	CNFGFlushRender();
	glGetIntegerv( GL_SCISSOR_BOX, xywh );
}

#endif // CNFGCONTEXTONLY
#endif // CNFG_WASM
