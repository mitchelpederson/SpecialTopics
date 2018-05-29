#include "Engine/Renderer/SpriteAnim.hpp"


SpriteAnim::SpriteAnim( const SpriteAnimDefinition* definition ) {

	m_definition = definition;

}


void SpriteAnim::Update(float deltaSeconds) {
	m_elapsedTime += deltaSeconds;

	if (m_elapsedTime >= m_definition->m_durationSeconds) {

		if (m_definition->m_isLooping) {
			m_elapsedTime -= m_definition->m_durationSeconds;
		}

		else {
			m_isPlaying = false;
		}
	}
}


AABB2 SpriteAnim::GetCurrentTexCoords() const {

	float fractionInto = m_elapsedTime / m_definition->m_durationSeconds;
	int numOfFrames = m_definition->GetNumOfFrames();
	int currentFrame = (int) (fractionInto * (float) numOfFrames);

	while (currentFrame >= numOfFrames) {
		currentFrame -= numOfFrames;
	}

	return m_definition->m_spriteSheet->GetTexCoordsForSpriteIndex(m_definition->m_frameIndices[currentFrame]);

}


Texture* SpriteAnim::GetTexture() const {
	return m_definition->m_spriteSheet->GetTexture();
}


void SpriteAnim::Pause() {
	m_isPlaying = false;
}


void SpriteAnim::Resume() {
	m_isPlaying = true;
}


void SpriteAnim::Reset() {
	m_isPlaying = false;
	m_elapsedTime = 0.f;
}


bool SpriteAnim::IsFinished() const {
	
	if (!m_definition->m_isLooping) {
		return !m_isPlaying;
	}
	else {
		return false;
	}
}

bool SpriteAnim::IsPlaying() const {
	return m_isPlaying;
}


float SpriteAnim::GetDurationSeconds() const {
	return m_definition->m_durationSeconds;
}


float SpriteAnim::GetSecondsElapsed() const {
	return m_elapsedTime;
}


float SpriteAnim::GetSecondsRemaining() const {
	return m_definition->m_durationSeconds - m_elapsedTime;
}


float SpriteAnim::GetFractionElapsed() const {
	return m_elapsedTime / m_definition->m_durationSeconds;
}


float SpriteAnim::GetFractionRemaining() const {
	return GetSecondsRemaining() / m_definition->m_durationSeconds;
}


void SpriteAnim::SetSecondsElapsed( float secondsElapsed ) {
	
	m_elapsedTime = secondsElapsed;

	while (m_elapsedTime > m_definition->m_durationSeconds) {
		m_elapsedTime -= m_definition->m_durationSeconds;
	}
}


void SpriteAnim::SetFractionElapsed( float fractionElapsed ) {

	m_elapsedTime = fractionElapsed * m_definition->m_durationSeconds;

	while (m_elapsedTime > m_definition->m_durationSeconds) {
		m_elapsedTime -= m_definition->m_durationSeconds;
	}
}
