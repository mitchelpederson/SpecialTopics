#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include <vector>




class SpriteAnim
{
public:
	SpriteAnim( const SpriteAnimDefinition* definition );

	void Update( float deltaSeconds );
	AABB2 GetCurrentTexCoords() const;	// Based on the current elapsed time
	Texture* GetTexture() const;
	void Pause();					// Starts unpaused (playing) by default
	void Resume();					// Resume after pausing
	void Reset();					// Rewinds to time 0 and starts (re)playing
	bool IsFinished() const;
	bool IsPlaying() const;
	float GetDurationSeconds() const;
	float GetSecondsElapsed() const;
	float GetSecondsRemaining() const;
	float GetFractionElapsed() const;
	float GetFractionRemaining() const;
	void SetSecondsElapsed( float secondsElapsed );	    // Jump to specific time
	void SetFractionElapsed( float fractionElapsed );    // e.g. 0.33f for one-third in


private:

	const SpriteAnimDefinition* m_definition;

	float m_elapsedTime = 0.f;
	bool m_isPlaying = true;

};
