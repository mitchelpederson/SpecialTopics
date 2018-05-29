//-----------------------------------------------------------------------------------------------
// Texture.cpp copied from example code on Canvas
//


#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/glbindings.h"
#include "Engine/ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <iostream>

//-----------------------------------------------------------------------------------------------
// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
//
Texture::Texture( const std::string& imageFilePath )
	: m_textureID( 0 )
	, m_dimensions( 0, 0 )
{
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)

									// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	PopulateFromData( imageData, m_dimensions, numComponents );
	stbi_image_free( imageData );
}

Texture::Texture()
	: m_textureID( 0 )
	, m_dimensions( 0, 0 )
{

}

//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents )
{
	
	m_dimensions = texelSize;
	int bytesX = m_dimensions.x * numComponents;

	for ( int row = 0; row < m_dimensions.y / 2; row++ ) {
		int oppositeRow = m_dimensions.y - row - 1;

		for ( int col = 0; col < bytesX; col++ ) {
			int currentIndex = bytesX * row + col;
			int oppositeIndex = bytesX * oppositeRow + col;

			unsigned char temp = imageData[ currentIndex ];
			imageData[ currentIndex ] = imageData[ oppositeIndex ];
			imageData[ oppositeIndex ] = temp;
		}
	}

	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Enable texturing
	//glEnable( GL_TEXTURE_2D );

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, m_textureID );

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); // GL_CLAMP or GL_REPEAT
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); // GL_CLAMP or GL_REPEAT

														   // Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if( numComponents == 3 )
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to be on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		m_dimensions.x,			// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,11], and B is the border thickness [0,1]
		m_dimensions.y,			// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,11], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1, recommend 0)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color channel/component)
		imageData );		// Address of the actual pixel data bytes/buffer in system memory

	while ((error = glGetError()) != GL_NO_ERROR) {
		std::cout << "GL Error: " << error << std::endl;
	}
}


int Texture::GetTextureID() const {
	return m_textureID;
}


bool Texture::CreateRenderTarget( int width, int height, eTextureFormat format )
{
	// generate the link to this texture
	glGenTextures( 1, &m_textureID ); 
	if (m_textureID == NULL) {
		return false; 
	}

	// TODO - add a TextureFormatToGLFormats( GLenum*, GLenum*, GLenum*, eTextureFormat )
	//        when more texture formats are required; 
	GLenum internal_format = GL_RGBA8; 
	GLenum channels = GL_RGBA;  
	GLenum pixel_layout = GL_UNSIGNED_BYTE;  
	if (format == TEXTURE_FORMAT_D24S8) {
		internal_format = GL_DEPTH_STENCIL; 
		channels = GL_DEPTH_STENCIL; 
		pixel_layout = GL_UNSIGNED_INT_24_8	; 
	}

	// Copy the texture - first, get use to be using texture unit 0 for this; 
	glActiveTexture( GL_TEXTURE0 ); 
	glBindTexture( GL_TEXTURE_2D, m_textureID );    // bind our texture to our current texture unit (0)

												 // Copy data into it;
	glTexImage2D( GL_TEXTURE_2D, 0, 
		internal_format, // what's the format OpenGL should use
		width, 
		height,        
		0,             // border, use 0
		channels,      // how many channels are there?
		pixel_layout,  // how is the data laid out
		nullptr );     // don't need to pass it initialization data 

	// cleanup after myself; 
	glBindTexture( GL_TEXTURE_2D, NULL ); // unset it; 

		// Save this all off
	m_dimensions.x = width;  
	m_dimensions.y = height; 

	m_format = format; // I save the format with the texture
					// for sanity checking.

	return true;
}


IntVector2 Texture::GetDimensions() const {
	return m_dimensions;
}


bool Texture::IsCubemap() const {
	return m_isCubemap;
}


Texture* Texture::CreateDuplicateTarget( Texture* copy ) {
	Texture* duplicate = new Texture();
	duplicate->CreateRenderTarget(copy->GetDimensions().x, copy->GetDimensions().y, copy->m_format);
	return duplicate;
}