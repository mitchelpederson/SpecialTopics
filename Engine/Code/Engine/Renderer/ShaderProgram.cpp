#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/glbindings.h"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <string>
#include <fstream>
#include <sstream>


// static unsigned int LoadShader( char const *filename, GLenum type );
// static GLuint CreateAndLinkProgram(GLint vs, GLint fs);



ShaderProgram::ShaderProgram() {

}


ShaderProgram::~ShaderProgram() {

}


bool ShaderProgram::LoadFromFiles( const char* root ) {

	std::string vs_file = root;
	vs_file += ".vs"; 

	std::string fs_file = root; 
	fs_file += ".fs"; 

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShader( vs_file.c_str(), GL_VERTEX_SHADER ); 
	GLuint frag_shader = LoadShader( fs_file.c_str(), GL_FRAGMENT_SHADER ); 

	// Link the program
	// program_handle is a member GLuint. 
	program_handle =  CreateAndLinkProgram( vert_shader, frag_shader ); 

	glDeleteShader( vert_shader ); 
	glDeleteShader( frag_shader ); 

	return (program_handle != NULL); 
}


bool ShaderProgram::LoadFromFiles( const char* vs, const char* fs ) {

	

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShader( vs, GL_VERTEX_SHADER ); 
	GLuint frag_shader = LoadShader( fs, GL_FRAGMENT_SHADER ); 

	// Link the program
	// program_handle is a member GLuint. 
	program_handle =  CreateAndLinkProgram( vert_shader, frag_shader ); 

	glDeleteShader( vert_shader ); 
	glDeleteShader( frag_shader ); 

	return (program_handle != NULL); 
}


bool ShaderProgram::LoadFromString(const char* vertShaderText, const char* fragShaderText) {
	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;

	// Create a shader
	GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(vertShaderText);
	glShaderSource(vertShaderID, 1, &vertShaderText, &shader_length);
	glCompileShader(vertShaderID);

	// Check status
	GLint vertStatus = 1;
	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &vertStatus);
	if (vertStatus == GL_FALSE) {
		LogShaderError(vertShaderID); // function we write
		glDeleteShader(vertShaderID);
		vertShaderID = NULL;
	}

	GLuint fragShaderID = glCreateShader( GL_FRAGMENT_SHADER ); 

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint frag_shader_length = (GLint)strlen(fragShaderText);
	glShaderSource(fragShaderID, 1, &fragShaderText, &frag_shader_length);
	glCompileShader(fragShaderID);

	// Check status
	GLint fragStatus = 1;
	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &fragStatus);
	if (fragStatus == GL_FALSE) {
		LogShaderError(fragShaderID); // function we write
		glDeleteShader(fragShaderID);
		fragShaderID = NULL;
	}

	// Link the program
	// program_handle is a member GLuint. 
	program_handle =  CreateAndLinkProgram( vertShaderID, fragShaderID ); 

	glDeleteShader( vertShaderID ); 
	glDeleteShader( fragShaderID ); 

	return (program_handle != NULL); 
}


GLuint ShaderProgram::CreateAndLinkProgram( GLint vs, GLint fs )
{
	// credate the program handle - how you will reference
	// this program within OpenGL, like a texture handle
	GLuint program_id = glCreateProgram();
	GUARANTEE_OR_DIE( program_id != 0, "glCreateProgram failed!" );

	// Attach the shaders you want to use
	glAttachShader( program_id, vs );
	glAttachShader( program_id, fs );

	// Link the program (create the GPU program)
	glLinkProgram( program_id );

	// Check for link errors - usually a result
	// of incompatibility between stages.
	GLint link_status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

	if (link_status == GL_FALSE) {
		LogProgramError(program_id);
		glDeleteProgram(program_id);
		program_id = 0;
	} 

	// no longer need the shaders, you can detach them if you want
	// (not necessary)
	glDetachShader( program_id, vs );
	glDetachShader( program_id, fs );

	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		DebuggerPrintf("GL Error: %d", error);
	}

	return program_id;
}


void ShaderProgram::LogShaderError(GLuint shader_id)
{
	// figure out how large the buffer needs to be
	GLint length;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

	// make a buffer, and copy the log to it. 
	char *buffer = new char[length + 1];
	glGetShaderInfoLog(shader_id, length, &length, buffer);

	// Print it out (may want to do some additional formatting)
	buffer[length] = NULL;
	//Logf( "class", buffer );
	ERROR_RECOVERABLE(buffer);
	DebuggerPrintf("%s", buffer);

	// free up the memory we used. 
	delete buffer;
}

unsigned int ShaderProgram::LoadShader( char const *filename, GLenum type )
{
	std::fstream fileStream(filename, std::fstream::in);
	GUARANTEE_OR_DIE(fileStream.is_open(), "Could not open shader file");

	std::stringstream buffer;
	buffer << fileStream.rdbuf();

	std::string fileString = buffer.str();

	const char* src = fileString.c_str();
	GUARANTEE_RECOVERABLE(src != nullptr, "Couldn't load shader");

	// Create a shader
	GLuint shader_id = glCreateShader(type);
	GUARANTEE_RECOVERABLE(shader_id != NULL, "Couldn't create shader");

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(src);
	glShaderSource(shader_id, 1, &src, &shader_length);
	glCompileShader(shader_id);

	// Check status
	GLint status = 1;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		LogShaderError(shader_id); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
		DebuggerPrintf("Failed to load shader from %s \n", filename);
	}

	fileStream.close();

	return shader_id;
}


void ShaderProgram::LogProgramError(GLuint program_id)
{
	// get the buffer length
	GLint length;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

	// copy the log into a new buffer
	char *buffer = new char[length + 1];
	glGetProgramInfoLog(program_id, length, &length, buffer);

	// print it to the output pane
	buffer[length] = NULL;
	ERROR_AND_DIE(buffer);

	// cleanup
	delete buffer;

}


unsigned int ShaderProgram::GetHandle() const {
	return program_handle;
}