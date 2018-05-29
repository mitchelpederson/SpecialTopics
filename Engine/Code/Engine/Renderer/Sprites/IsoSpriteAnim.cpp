#include "Engine/Renderer/Sprites/IsoSpriteAnim.hpp"


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