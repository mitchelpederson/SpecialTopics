#pragma once
#include <string>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>

class Image {

public:
	Image();
	explicit Image( const std::string& imageFilePath );
	Rgba	GetTexel( int x, int y ) const; 			// (0,0) is top-left
	void	SetTexel( int x, int y, const Rgba& color );
	IntVector2 GetDimensions() const;
	void	InvertY();
	int		GetNumComponents();

	unsigned char* GetAsData();
	unsigned char* ExtractSquareAtOffset( unsigned int size, unsigned int xOffset, unsigned int yOffset );


private:

	void PopulateFromData( unsigned char* data );
	IntVector2		m_dimensions;
	int				m_numComponents;
	std::vector< Rgba >	m_texels;

};