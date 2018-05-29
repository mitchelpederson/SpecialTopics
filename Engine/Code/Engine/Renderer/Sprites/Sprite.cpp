#include "Engine/Renderer/Sprites/Sprite.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

std::map<std::string, Sprite*> Sprite::s_sprites;

Sprite::Sprite( const tinyxml2::XMLElement& xml ) {

	std::string name = "error";
	name = ParseXmlAttribute(xml, "id", name);

	if (name != "error") {
		s_sprites[name] = this;

		const tinyxml2::XMLElement* diffuseElement = xml.FirstChildElement("diffuse");
		const tinyxml2::XMLElement* ppuElement = xml.FirstChildElement("ppu");
		const tinyxml2::XMLElement* uvElement = xml.FirstChildElement("uv");
		const tinyxml2::XMLElement* pivotElement = xml.FirstChildElement("pivot");

		float m_ppu = ppuElement->FloatText();
		std::string spriteSheetName = "error";
		spriteSheetName = ParseXmlAttribute(*diffuseElement, "src", spriteSheetName);
		m_spriteSheet = g_theRenderer->CreateOrGetTexture(spriteSheetName);

		m_spriteSheet = g_theRenderer->CreateOrGetTexture(spriteSheetName);

		std::string pivotString = pivotElement->GetText();
		m_pivot.SetFromText(pivotString.c_str());

		std::string uvString = uvElement->GetText();
		m_uvs.SetFromText(uvString.c_str()); // In pixels
		float tempMaxY = m_uvs.maxs.y;
		m_uvs.maxs.y = m_spriteSheet->GetDimensions().y - m_uvs.mins.y;
		m_uvs.mins.y = m_spriteSheet->GetDimensions().y - tempMaxY;

		m_dimensions = Vector2((m_uvs.maxs.x - m_uvs.mins.x) * ( 1.f / m_ppu), (m_uvs.maxs.y - m_uvs.mins.y) * ( 1.f / m_ppu)); // in world units
		
		float tempX = m_uvs.mins.x;
		m_uvs.mins.x = m_uvs.maxs.x;
		m_uvs.maxs.x = tempX;
		
		m_uvs.mins.x = m_uvs.mins.x / m_spriteSheet->GetDimensions().x; // in uv coords
		m_uvs.maxs.x = m_uvs.maxs.x / m_spriteSheet->GetDimensions().x;
		m_uvs.mins.y = m_uvs.mins.y / m_spriteSheet->GetDimensions().y;
		m_uvs.maxs.y = m_uvs.maxs.y / m_spriteSheet->GetDimensions().y;

	}
	
	else {
		ERROR_AND_DIE("Couldn't load a sprite that didn't have an id");
	}
}


Sprite::Sprite( const Sprite& copy ) 
	: m_dimensions( copy.GetDimensions() )
	, m_spriteSheet( copy.GetTexture() )
	, m_uvs( copy.GetUVs() )
	, m_pivot( copy.GetPivot() )
	, m_ppu( copy.GetPPU() )
	, m_scale( copy.GetScale() )
{
}


void Sprite::SetScale( const Vector2& scale ) {
	m_scale = scale;
}


float Sprite::GetPPU() const {
	return m_ppu;
}

AABB2 Sprite::GetUVs() const {
	return m_uvs;
}

Texture* Sprite::GetTexture() const {
	return m_spriteSheet;
}
Vector2 Sprite::GetDimensions() const {
	return m_dimensions;
}

Vector2 Sprite::GetPivot() const {
	return m_pivot;
}

Vector2 Sprite::GetScale() const {
	return m_scale;
}