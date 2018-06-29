#include "Engine/Audio/AudioCue.hpp"
#include "Engine/Core/EngineCommon.hpp"


AudioCue::AudioCue( AudioCueDefinition* def ) 
	: m_def(def)
{
}


void AudioCue::Play( float volume/* =1.f */, float balance/* =0.0f */, float speed/* =1.0f */, bool isPaused/* =false */ ) {
	m_currentlyPlayingSound = g_audioSystem->PlaySound( m_def->GetRandomSoundFromCue(), false, volume, balance, speed, isPaused );
}


void AudioCue::Stop() {
	g_audioSystem->StopSound(m_currentlyPlayingSound);
	m_currentlyPlayingSound = (SoundPlaybackID) -1;
}