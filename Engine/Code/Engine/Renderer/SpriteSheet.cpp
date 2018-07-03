#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

SpriteSheet::SpriteSheet() : m_sheetTexture(nullptr), m_spriteLayout(IntVector2(1, 1)) 
{
	ERROR_AND_DIE("Called default SpriteSheet constructor");
}


SpriteSheet::SpriteSheet(Texture* texture, const IntVector2& size) : m_sheetTexture(texture), m_spriteLayout(size) 
{
	GUARANTEE_OR_DIE(size.x > 0, "SpriteSheet wasn't bigger than 0");
}


SpriteSheet::SpriteSheet(const SpriteSheet& copy) {
	m_sheetTexture = copy.GetTexture();
	m_spriteLayout = copy.GetSpriteLayout();
}


SpriteSheet::~SpriteSheet() {

}


Texture* SpriteSheet::GetTexture() const {
	return m_sheetTexture;
}


int SpriteSheet::GetNumSprites() const {
	return m_spriteLayout.x * m_spriteLayout.y;
}


AABB2 SpriteSheet::GetTexCoordsForSprteCoords(const IntVector2& spriteCoords) const {

	IntVector2 invertedCoords(spriteCoords.x, m_spriteLayout.y - spriteCoords.y - 1);

	Vector2 mins(((float) invertedCoords.x / (float) m_spriteLayout.x) + 0.001f, ((float) invertedCoords.y / (float) m_spriteLayout.y) + 0.001f);
	Vector2 maxs(((float) (invertedCoords.x + 1) / (float) m_spriteLayout.x) - 0.001f, ((float) (invertedCoords.y + 1) / (float) m_spriteLayout.y) - 0.001f);
	return AABB2(mins, maxs);
}


AABB2 SpriteSheet::GetTexCoordsForSpriteIndex(int spriteIndex) const {

	IntVector2 spriteCoords(spriteIndex % m_spriteLayout.x, spriteIndex / m_spriteLayout.x);
	return GetTexCoordsForSprteCoords(spriteCoords);
}


IntVector2 SpriteSheet::GetSpriteLayout() const {
	return m_spriteLayout;
}