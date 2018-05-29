#pragma once
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include <map>

class SpriteAnimSetDefinition {

	friend class SpriteAnimSet;
	friend class Renderer;

public:

	SpriteAnimSetDefinition( const tinyxml2::XMLElement& xml );
	~SpriteAnimSetDefinition();
	bool IsAdditiveBlending() const;


private:

	std::map<std::string, SpriteAnimDefinition*> m_animDefs;
	std::string m_defaultAnimName = "Idle";
	bool m_additiveBlend = false;
};