#include "Engine/Core/Image.hpp"
#include "Engine/ThirdParty/stb/stb_image.h"

Image::Image()
{

}

Image::Image(const std::string& imageFilePath) {

	unsigned char* imageData = stbi_load(imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &m_numComponents, 0);

	PopulateFromData( imageData );
}


void Image::PopulateFromData( unsigned char* data ) {

	int index = 0;

	while (index < (m_dimensions.x * m_dimensions.y) * m_numComponents) {

		Rgba texel(data[index], data[index + 1], data[index + 2], 255);

		if (m_numComponents == 4) {
			texel.a = data[index + 3];
			index += 1;
		}

		m_texels.push_back(texel);
		index += 3;

	}
}

void Image::SetTexel(int x, int y, const Rgba& color) {
	m_texels[(y * m_dimensions.x) + x] = color;
}


Rgba Image::GetTexel(int x, int y) const {
	return m_texels[(y * m_dimensions.x) + x];
}


void Image::InvertY() {

	for ( int row = 0; row < m_dimensions.y / 2; row++ ) {
		int oppositeRow = m_dimensions.y - row - 1;

		for ( int col = 0; col < m_dimensions.x; col++ ) {
			int currentIndex = m_dimensions.x * row + col;
			int oppositeIndex = m_dimensions.x * oppositeRow + col;

			Rgba temp( m_texels[ currentIndex ] );
			m_texels[ currentIndex ] = m_texels[ oppositeIndex ];
			m_texels[ oppositeIndex ] = temp;
		}
	}

}

IntVector2 Image::GetDimensions() const {
	return m_dimensions;
}


unsigned char* Image::GetAsData() {
	return (unsigned char*) m_texels.data();
}


unsigned char* Image::ExtractSquareAtOffset( unsigned int size, unsigned int xOffset, unsigned int yOffset ) {
	std::vector<Rgba> square;

	for (int y = yOffset; y < yOffset + size; y++) {	
		for (int x = xOffset; x < xOffset + size; x++) {
			int index = (y * size) + x;
			square.push_back(GetTexel(x, y));
		}
	}

	unsigned char* data = new unsigned char[square.size() * 4];
	memcpy(data, square.data(), square.size() * 4);

	return data;

}


int Image::GetNumComponents() {
	return m_numComponents;
}