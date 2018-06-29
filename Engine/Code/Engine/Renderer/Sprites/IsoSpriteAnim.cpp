#include "Engine/Renderer/Sprites/IsoSpriteAnim.hpp"
#include "Engine/Renderer/Sprites/IsoSpriteAnimDef.hpp"

IsoSpriteAnim::IsoSpriteAnim( IsoSpriteAnimDef* def, Clock* parent ) {
	m_clock = new Clock(GetMasterClock());
	m_def = def;
	m_startTime = m_clock->total.seconds;
}


void IsoSpriteAnim::Pause() {
	m_clock->SetPaused(true);
}

void IsoSpriteAnim::Unpause() {
	m_clock->SetPaused(false);
}

void IsoSpriteAnim::SetPlaybackSpeed( float scale ) {
	m_clock->SetScale(scale);
}

void IsoSpriteAnim::Reset() {
	m_startTime = m_clock->total.seconds;
}


IsoSprite* IsoSpriteAnim::GetCurrentIsoSprite() const {
	float currentTime = m_clock->total.seconds - m_startTime;
	return m_def->GetIsoSpriteForTime(currentTime);
}


bool IsoSpriteAnim::HasFinished() const {
	if (m_def->GetMode() == IsoSpriteAnimDef::ANIM_MODE_LOOP) {
		return false;
	}
	return (m_clock->total.seconds - m_startTime) >= m_def->GetDuration();
}