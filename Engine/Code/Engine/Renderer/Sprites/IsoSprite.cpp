#include "Engine/Renderer/Sprites/IsoSprite.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"


std::map<std::string, IsoSprite*> IsoSprite::s_isoSprites;


IsoSprite::IsoSprite( const tinyxml2::XMLElement& xml ) {

	m_name = ParseXmlAttribute( xml, "id", m_name );
	const tinyxml2::XMLElement* sprite = xml.FirstChildElement("facing");

	while (sprite != nullptr) {
		IntVector2 direction(0,0);
		Vector2 scale(0.f, 0.f);
		std::string spriteSrc = "sprite source error";

		direction = ParseXmlAttribute(*sprite, "dir", direction);
		spriteSrc = ParseXmlAttribute(*sprite, "src", spriteSrc);
		scale     = ParseXmlAttribute(*sprite, "scale", scale);

		Sprite* spriteToAdd = nullptr;
		if (scale == Vector2(0.f,0.f)) {
			spriteToAdd = Sprite::s_sprites[spriteSrc];
		}
		else {
			spriteToAdd = new Sprite(*Sprite::s_sprites[spriteSrc]);
			spriteToAdd->SetScale(scale);
		}

		if (direction == IntVector2::AWAYRIGHT) {
			m_sprites[ISO_AWAY_RIGHT] = spriteToAdd;
		}
		else if (direction == IntVector2::AWAYLEFT) {
			m_sprites[ISO_AWAY_LEFT] = spriteToAdd;
		}
		else if (direction == IntVector2::TOWARDLEFT) {
			m_sprites[ISO_TOWARDS_LEFT] = spriteToAdd;
		}
		else if (direction == IntVector2::TOWARDRIGHT) {
			m_sprites[ISO_TOWARDS_RIGHT] = spriteToAdd;
		}

		s_isoSprites[m_name] = this;
		sprite = sprite->NextSiblingElement("facing");
	}
}


Sprite* IsoSprite::GetSpriteForViewAngle( const Vector3& actorDirection, const Vector3& cameraForward, const Vector3& cameraRight ) {

	Vector3 actorDirNormalized = actorDirection.GetNormalized();
	float dotActorCameraForward = DotProduct(actorDirNormalized, cameraForward);
	float dotActorCameraRight = DotProduct(actorDirNormalized, cameraRight);

	if ( dotActorCameraForward >= 0.f) {
		if ( dotActorCameraRight >= 0.f) {
			return m_sprites[ISO_TOWARDS_RIGHT];
		}
		else {
			return m_sprites[ISO_AWAY_LEFT];
		}
	}

	else {
		if ( dotActorCameraRight >= 0.f) {
			return m_sprites[ISO_AWAY_RIGHT];
		}
		else {
			return m_sprites[ISO_TOWARDS_LEFT];
		}
	}



}