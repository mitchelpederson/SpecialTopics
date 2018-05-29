#pragma once

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVector2.hpp"

class SpriteSheet {
public:
	SpriteSheet();
	SpriteSheet(Texture* texture, const IntVector2& size);

	SpriteSheet(const SpriteSheet& copy);
	~SpriteSheet();

	Texture* GetTexture() const;

	int GetNumSprites() const;
	IntVector2 GetSpriteLayout() const;

	AABB2 GetTexCoordsForSprteCoords(const IntVector2& spriteCoords) const;
	AABB2 GetTexCoordsForSpriteIndex(int spriteIndex) const;


private:
	Texture* m_sheetTexture;
	IntVector2 m_spriteLayout;

};

