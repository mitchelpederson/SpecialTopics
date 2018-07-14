#pragma once

#if !defined( ENGINE_DISABLE_AUDIO )

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vector3.hpp"
#include "Engine/ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>


//-----------------------------------------------------------------------------------------------
typedef size_t SoundID;
typedef size_t SoundPlaybackID;
constexpr size_t MISSING_SOUND_ID = (size_t)(-1); // for bad SoundIDs and SoundPlaybackIDs


//-----------------------------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

public:
	virtual void				BeginFrame();
	virtual void				EndFrame();

	virtual SoundID				CreateOrGetSound( const std::string& soundFilePath, bool is3D = true );
	virtual SoundPlaybackID		PlaySound( SoundID soundID, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual SoundPlaybackID		PlaySoundAtLocation( SoundID, const Vector3& position, const Vector3& velocity, bool isLooped=false, float volume=1.f, float balance=0.0f, float speed=1.0f, bool isPaused=false );
	virtual void				StopSound( SoundPlaybackID soundPlaybackID );
	virtual void				SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume );	// volume is in [0,1]
	virtual void				SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance );	// balance is in [-1,1], where 0 is L/R centered
	virtual void				SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed );		// speed is frequency multiplier (1.0 == normal)
	virtual void				SetListenerParameters( const Vector3& position, const Vector3& velocity, const Vector3& forward, const Vector3& up );

	virtual void				ValidateResult( FMOD_RESULT result );

	float						GetBassVolume();
	void						AddFFTToChannel( SoundPlaybackID sound );
	void						ActivateGlobalReverb();
	void						DeactivateGlobalReverb();

protected:
	virtual SoundID				CreateOrGet3DSound( const std::string& soundFilePath );
	virtual SoundID				CreateOrGet2DSound( const std::string& soundFilePath );


protected:

	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registered2DSoundIDs;
	std::map< std::string, SoundID >	m_registered3DSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;

	FMOD::DSP*							m_fftdsp;
	FMOD_DSP_PARAMETER_FFT*				m_fft;
};

#endif