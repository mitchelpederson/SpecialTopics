#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

class BitmapFont {
	friend class Renderer;

public:


	AABB2 GetUVsForGlyph( int glyphUnicode ) const; // pass ‘A’ or 65 for A, etc.
	float GetGlyphAspect( /*int glyphUnicode*/ ) const { return m_baseAspect; } // will change later
	float GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale );

	const Texture* GetFontTexture() const;

private:
	BitmapFont( const std::string& imageFilePath );

	SpriteSheet*	m_spriteSheet; // assumed 16x16 glyphs
	float			m_baseAspect;

};