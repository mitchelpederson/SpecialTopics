#pragma once
#include "Engine/Renderer/glbindings.h"

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	bool LoadFromFiles( char const *root );
	bool LoadFromString( const char* vertShaderText, const char* fragShaderText);
	bool LoadFromFiles( const char* vs, const char* fs );

	unsigned int GetHandle() const;

private:
	static unsigned int LoadShader( char const *filename, GLenum type );
	static void LogShaderError(GLuint shader_id);
	static void LogProgramError(GLuint program_id);
	static GLuint CreateAndLinkProgram( GLint vs, GLint fs );

	unsigned int program_handle = 0;
};