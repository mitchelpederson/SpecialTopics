#pragma once

#include "Engine/Renderer/Sprites/IsoSprite.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include <vector>

struct FrameDef {
	std::string isoSprite = "";
	float duration = 0.1f;
	std::string audioCueName = "";

	IsoSprite* GetSprite() {
		return IsoSprite::s_isoSprites[isoSprite];
	}
};


class IsoSpriteAnimDef {

public:
	enum eAnimMode {
		ANIM_MODE_LOOP,
		ANIM_MODE_CLAMP
	};

public:
	IsoSpriteAnimDef( const tinyxml2::XMLElement& xml );

	static std::map<std::string, IsoSpriteAnimDef*> s_definitions;

	IsoSprite* GetIsoSpriteForTime( float timeIntoAnim ) const;
	std::string GetFrameSoundForTime( float timeIntoAnim, float deltaSeconds ) const;
	float GetDuration() const;
	eAnimMode GetMode() const;
	const std::string& GetName() const;
	

private:
	std::string				m_name = "anim name error";
	eAnimMode				m_mode = ANIM_MODE_LOOP;
	float					m_duration = 0.f;
	std::vector<FrameDef*>	m_frames;

};