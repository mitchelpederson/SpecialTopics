#pragma once

#include "Engine/Renderer/Sprites/IsoSpriteAnimDef.hpp"
#include "Engine/Core/Clock.hpp"


class IsoSpriteAnim {

public:
	IsoSpriteAnim( IsoSpriteAnimDef* def, Clock* parent );

	void Pause();
	void Unpause();
	void SetPlaybackSpeed( float scale );
	void Reset();

	IsoSprite* GetCurrentIsoSprite() const;


public:
	IsoSpriteAnimDef* m_def;

	Clock* m_clock = nullptr;
	float m_startTime = 0.f;
};