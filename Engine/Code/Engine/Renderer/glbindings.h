#pragma once
#include "Engine/ThirdParty/gl/glcorearb.h"
#include "Engine/ThirdParty/gl/glext.h"
#include "Engine/ThirdParty/gl/wglext.h"

 extern HMODULE g_GLLibrary;

// Use this to deduce type of the pointer so we can cast; 
template <typename T>
bool wglGetTypedProcAddress( T *out, char const *name ) 
{
	// Grab the function from the currently bound render context
	// most opengl 2.0+ features will be found here
	*out = (T) wglGetProcAddress(name); 

	if ((*out) == nullptr) {
		// if it is not part of wgl (the device), then attempt to get it from the GLL library
		// (most OpenGL functions come from here)
		*out = (T) GetProcAddress( g_GLLibrary, name); 
	}

	return (*out != nullptr); 
}

#define GL_BIND_FUNCTION(name) wglGetTypedProcAddress( &name, #name )

extern PFNGLCLEARPROC						glClear;
extern PFNGLCLEARCOLORPROC					glClearColor;
extern PFNWGLGETEXTENSIONSSTRINGARBPROC		wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribsARB;
extern PFNGLENABLEPROC						glEnable;
extern PFNGLBLENDFUNCPROC					glBlendFunc;
extern PFNGLLINEWIDTHPROC					glLineWidth;
extern PFNGLSHADERSOURCEPROC				glShaderSource;
extern PFNGLCOMPILESHADERPROC				glCompileShader;
extern PFNGLCREATESHADERPROC				glCreateShader;
extern PFNGLGETSHADERIVPROC					glGetShaderiv;
extern PFNGLDELETESHADERPROC				glDeleteShader;
extern PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC				glCreateProgram;
extern PFNGLATTACHSHADERPROC				glAttachShader;
extern PFNGLLINKPROGRAMPROC					glLinkProgram;
extern PFNGLGETPROGRAMIVARBPROC				glGetProgramiv;
extern PFNGLDELETEPROGRAMPROC				glDeleteProgram;
extern PFNGLDETACHSHADERPROC				glDetachShader;
extern PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;
extern PFNGLGENBUFFERSPROC					glGenBuffers;
extern PFNGLBINDBUFFERPROC					glBindBuffer;
extern PFNGLBUFFERDATAPROC					glBufferData;
extern PFNGLDELETEBUFFERSPROC				glDeleteBuffers;
extern PFNGLGENVERTEXARRAYSPROC				glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC				glBindVertexArray;
extern PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation;		
extern PFNGLENABLEVERTEXATTRIBARRAYPROC		glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC			glVertexAttribPointer;
extern PFNGLUSEPROGRAMPROC					glUseProgram;
extern PFNGLDRAWARRAYSPROC					glDrawArrays;
extern PFNGLBINDTEXTUREPROC					glBindTexture;
extern PFNGLDISABLEPROC						glDisable;	
extern PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
extern PFNGLPROGRAMUNIFORM4FVPROC			glProgramUniform4fv;
extern PFNGLPROGRAMUNIFORMMATRIX4FVPROC		glProgramUniformMatrix4fv;
extern PFNGLSAMPLERPARAMETERIPROC			glSamplerParameteri;
extern PFNGLGENSAMPLERSPROC					glGenSamplers;	
extern PFNGLDELETESAMPLERSPROC				glDeleteSamplers;
extern PFNGLPIXELSTOREIPROC					glPixelStorei; 
extern PFNGLGENTEXTURESPROC					glGenTextures; 
extern PFNGLTEXPARAMETERIPROC				glTexParameteri;
extern PFNGLTEXIMAGE2DPROC					glTexImage2D;
extern PFNGLGETERRORPROC					glGetError;
extern PFNGLBINDSAMPLERPROC					glBindSampler;
extern PFNGLACTIVETEXTUREPROC				glActiveTexture;
extern PFNGLUNIFORM1IPROC					glUniform1i;
extern PFNGLPOLYGONMODEPROC					glPolygonMode;
extern PFNGLGENFRAMEBUFFERSPROC				glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC			glDeleteFramebuffers;
extern PFNGLBINDFRAMEBUFFERPROC				glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC			glFramebufferTexture;
extern PFNGLDRAWBUFFERSPROC					glDrawBuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC		glCheckFramebufferStatus;
extern PFNGLBLITFRAMEBUFFERPROC				glBlitFramebuffer;
extern PFNGLDEPTHFUNCPROC					glDepthFunc;
extern PFNGLDEPTHMASKPROC					glDepthMask;
extern PFNGLCLEARDEPTHFPROC					glClearDepthf;
extern PFNGLDRAWELEMENTSPROC				glDrawElements;
extern PFNGLCULLFACEPROC					glCullFace;
extern PFNGLFRONTFACEPROC					glFrontFace;
extern PFNGLUNIFORM4FPROC					glUniform4f;
extern PFNGLBLENDEQUATIONPROC				glBlendEquation;
extern PFNGLUNIFORM1FPROC					glUniform1f;
extern PFNGLUNIFORM3FPROC					glUniform3f;
extern PFNGLUNIFORM4FVPROC					glUniform4fv;
extern PFNGLUNIFORM3FVPROC					glUniform3fv;
extern PFNGLUNIFORM1FVPROC					glUniform1fv;
extern PFNGLBLENDFUNCSEPARATEPROC			glBlendFuncSeparate;
extern PFNGLTEXSTORAGE2DPROC				glTexStorage2D;
extern PFNGLTEXSUBIMAGE2DPROC				glTexSubImage2D;
extern PFNGLCLEARBUFFERIVPROC				glClearBufferiv;
extern PFNGLCLEARBUFFERFVPROC				glClearBufferfv;
extern PFNGLREADPIXELSPROC					glReadPixels;


void BindGLFunctions();
