//-----------------------------------------------------------------------------------------------
// Texture.cpp copied from example code on Canvas
//


#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Renderer/glbindings.h"
#include "Engine/ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <iostream>

//-----------------------------------------------------------------------------------------------
// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
//
CubeMap::CubeMap( const std::string& imageFilePath ) : m_image( imageFilePath )
{
 
	m_dimensions = m_image.GetDimensions();
	m_isCubemap = true;
	
	m_size = m_dimensions.x / 4;
	m_image.InvertY();
	PopulateFromData( m_image.GetAsData(), m_dimensions, m_image.GetNumComponents() );
}

CubeMap::CubeMap()
{

}

//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void CubeMap::PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents )
{

	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
		bufferFormat = GL_RGB;

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_textureID );
	glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, m_size, m_size ); 

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); // GL_CLAMP or GL_REPEAT
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); // GL_CLAMP or GL_REPEAT
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
																		   // Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	unsigned char* rightFaceData	= m_image.ExtractSquareAtOffset( m_size, m_size * 2, m_size * 1 );
	unsigned char* leftFaceData		= m_image.ExtractSquareAtOffset( m_size, m_size * 0, m_size * 1 );
	unsigned char* backFaceData		= m_image.ExtractSquareAtOffset( m_size, m_size * 1, m_size * 1 );
	unsigned char* frontFaceData	= m_image.ExtractSquareAtOffset( m_size, m_size * 3, m_size * 1 );
	unsigned char* topFaceData		= m_image.ExtractSquareAtOffset( m_size, m_size * 1, m_size * 0 );
	unsigned char* bottomFaceData	= m_image.ExtractSquareAtOffset( m_size, m_size * 1, m_size * 2 );

	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, m_size,	m_size,	bufferFormat, GL_UNSIGNED_BYTE, rightFaceData );		
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, 0, 0, m_size,	m_size,	bufferFormat, GL_UNSIGNED_BYTE, leftFaceData );		
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, 0, 0, m_size,	m_size,	bufferFormat, GL_UNSIGNED_BYTE, topFaceData );		
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, 0, 0, m_size,	m_size,	bufferFormat, GL_UNSIGNED_BYTE, bottomFaceData );		
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, 0, 0, m_size,	m_size,	bufferFormat, GL_UNSIGNED_BYTE, backFaceData );		
	glTexSubImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, 0, 0, m_size,	m_size,	bufferFormat, GL_UNSIGNED_BYTE, frontFaceData );		

	delete[] rightFaceData;
	delete[] leftFaceData;
	delete[] frontFaceData;
	delete[] backFaceData;
	delete[] topFaceData;
	delete[] bottomFaceData;

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}
}
