#pragma once 
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Texture.hpp"
#include <string>


enum eCubeMapFormat {
	CUBE_MAP_FORMAT_RGBA8,
	CUBE_MAP_FORMAT_D24S8
};

class CubeMap : public Texture {
	friend class Renderer;

public:
	CubeMap();
	~CubeMap();

private:
	CubeMap( const std::string& imageFilePath );
	virtual void PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents ) override;

	unsigned int	m_size;
	Image			m_image;

};