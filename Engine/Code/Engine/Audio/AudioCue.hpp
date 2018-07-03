#pragma once

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Audio/AudioCueDefinition.hpp"

class AudioCue {

public:
	AudioCue( AudioCueDefinition* def );

	void Play( float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	void Stop();


private:
	SoundPlaybackID m_currentlyPlayingSound = (SoundPlaybackID) -1;
	AudioCueDefinition* m_def;
};