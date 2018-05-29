#pragma once
#include "Engine/Renderer/Sprites/Sprite.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"



class IsoSprite {

	enum eIsoDirection {
		ISO_AWAY_RIGHT,
		ISO_AWAY_LEFT,
		ISO_TOWARDS_LEFT,
		ISO_TOWARDS_RIGHT,
		ISO_NUM_DIRECTIONS
	};

public:
	IsoSprite( const tinyxml2::XMLElement& xml );
	Sprite* GetSpriteForViewAngle( const Vector3& actorDirection, const Vector3& cameraForward, const Vector3& cameraRight );

	static std::map<std::string, IsoSprite*> s_isoSprites;

private:

	Sprite* m_sprites[ISO_NUM_DIRECTIONS];
	std::string m_name = "IsoSprite name error";

};