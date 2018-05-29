#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/EngineCommon.hpp"

BitmapFont::BitmapFont( const std::string& imageFilePath ) {
	m_spriteSheet = new SpriteSheet(g_theRenderer->CreateOrGetTexture(imageFilePath), IntVector2(16, 16));

	m_baseAspect = 1.f;

}


AABB2 BitmapFont::GetUVsForGlyph( int glyphUnicode ) const {

	return m_spriteSheet->GetTexCoordsForSpriteIndex(glyphUnicode);

}


float BitmapFont::GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale ) {

	float width = 0.f;

	for (unsigned int character = 0; character < asciiText.length(); character++ ) {
		width += (aspectScale * GetGlyphAspect(/*asciiText[character]*/)) * cellHeight;
	}

	return width;
}


const Texture* BitmapFont::GetFontTexture() const {
	return m_spriteSheet->GetTexture();
}