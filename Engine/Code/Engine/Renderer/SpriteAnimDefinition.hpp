#pragma once
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Renderer/SpriteSheet.hpp"
#include <vector>

class SpriteAnimDefinition {

	friend class SpriteAnim;
	friend class SpriteAnimSetDefinition;

public:
	SpriteAnimDefinition( const tinyxml2::XMLElement& xml, int defaultFPS);

	int					GetNumOfFrames() const;
	SpriteSheet*		GetSpriteSheet() const;
	float				SetDuration();
	bool				IsLooping() const;
	bool				isFinished() const;
	SpriteAnim*			GetSpriteAnim(const std::string& name);

private:
	SpriteSheet*		m_spriteSheet = nullptr;
	float				m_durationSeconds = 1.f;
	std::vector<int>	m_frameIndices;
	bool				m_isLooping = true;
	int					m_fps = 1;
};