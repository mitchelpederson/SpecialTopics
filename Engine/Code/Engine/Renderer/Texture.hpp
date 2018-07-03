//-----------------------------------------------------------------------------------------------
// Texture.hpp copied from example code on Canvas
//
#pragma once
#include "Engine/Math/IntVector2.hpp"
#include <string>

enum eTextureFormat {
	TEXTURE_FORMAT_RGBA8,
	TEXTURE_FORMAT_D24S8
};

class Texture {
	friend class Renderer;

public:
	Texture();
	~Texture();

	int GetTextureID() const;
	bool CreateRenderTarget( int width, int height, eTextureFormat format = TEXTURE_FORMAT_RGBA8 );
	IntVector2 GetDimensions() const;
	bool IsCubemap() const;

	static Texture* CreateDuplicateTarget( Texture* copy );

protected:
	Texture( const std::string& imageFilePath );
	virtual void PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents );

	unsigned int	m_textureID;
	unsigned int	m_mipCount;
	IntVector2		m_dimensions;
	eTextureFormat  m_format = TEXTURE_FORMAT_RGBA8;
	bool			m_isCubemap = false;

};