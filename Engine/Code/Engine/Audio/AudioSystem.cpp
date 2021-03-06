#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

//-----------------------------------------------------------------------------------------------
// To disable audio entirely (and remove requirement for fmod.dll / fmod64.dll) for any game,
//	#define ENGINE_DISABLE_AUDIO in your game's Code/Game/EngineBuildPreferences.hpp file.
//
// Note that this #include is an exception to the rule "engine code doesn't know about game code".
//	Purpose: Each game can now direct the engine via #defines to build differently for that game.
//	Downside: ALL games must now have this Code/Game/EngineBuildPreferences.hpp file.
//
#include "Game/EngineBuildPreferences.hpp"
#if !defined( ENGINE_DISABLE_AUDIO )


//-----------------------------------------------------------------------------------------------
// Link in the appropriate FMOD static library (32-bit or 64-bit)
//
#if defined( _WIN64 )
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod64_vc.lib" )
#else
#pragma comment( lib, "Engine/ThirdParty/fmod/fmod_vc.lib" )
#endif


//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
AudioSystem::AudioSystem()
	: m_fmodSystem( nullptr )
{
	FMOD_RESULT result;
	result = FMOD::System_Create( &m_fmodSystem );
	ValidateResult( result );

	result = m_fmodSystem->init( 512, FMOD_INIT_NORMAL, nullptr );
	ValidateResult( result );

	m_fmodSystem->createDSPByType(FMOD_DSP_TYPE::FMOD_DSP_TYPE_FFT, &m_fftdsp);
	m_fft = new FMOD_DSP_PARAMETER_FFT();

}


//-----------------------------------------------------------------------------------------------
AudioSystem::~AudioSystem()
{
	FMOD_RESULT result = m_fmodSystem->release();
	ValidateResult( result );

	m_fmodSystem = nullptr; // #Fixme: do we delete/free the object also, or just do this?
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::BeginFrame()
{
	m_fmodSystem->update();
	m_fftdsp->getParameterData(FMOD_DSP_FFT_SPECTRUMDATA, (void**) &m_fft, 0, 0, 0);

}


//-----------------------------------------------------------------------------------------------
void AudioSystem::EndFrame()
{
}


//-----------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGetSound( const std::string& soundFilePath, bool is3D /* = true */ )
{
	if ( is3D ) {
		return CreateOrGet3DSound(soundFilePath);
	}
	else {
		return CreateOrGet2DSound(soundFilePath);
	}
}


//----------------------------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGet3DSound( const std::string& soundFilePath ) {
	
	std::map< std::string, SoundID >::iterator found = m_registered3DSoundIDs.find( soundFilePath );
	if( found != m_registered3DSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_3D, nullptr, &newSound );

		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registered3DSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//----------------------------------------------------------------------------------------------------------------
SoundID AudioSystem::CreateOrGet2DSound( const std::string& soundFilePath ) {

	std::map< std::string, SoundID >::iterator found = m_registered2DSoundIDs.find( soundFilePath );
	if( found != m_registered2DSoundIDs.end() )
	{
		return found->second;
	}
	else
	{
		FMOD::Sound* newSound = nullptr;
		m_fmodSystem->createSound( soundFilePath.c_str(), FMOD_2D, nullptr, &newSound );
		if( newSound )
		{
			SoundID newSoundID = m_registeredSounds.size();
			m_registered2DSoundIDs[ soundFilePath ] = newSoundID;
			m_registeredSounds.push_back( newSound );
			return newSoundID;
		}
	}

	return MISSING_SOUND_ID;
}


//-----------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySound( SoundID soundID, bool isLooped, float volume, float balance, float speed, bool isPaused )
{
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, isPaused, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	return (SoundPlaybackID) channelAssignedToSound;
}


//----------------------------------------------------------------------------------------------------------------
SoundPlaybackID AudioSystem::PlaySoundAtLocation( SoundID soundID, const Vector3& position, const Vector3& velocity, bool isLooped/* =false */, float volume/* =1.f */, float balance/* =0.0f */, float speed/* =1.0f */, bool isPaused/* =false */ ) {
	size_t numSounds = m_registeredSounds.size();
	if( soundID < 0 || soundID >= numSounds )
		return MISSING_SOUND_ID;

	FMOD::Sound* sound = m_registeredSounds[ soundID ];
	if( !sound )
		return MISSING_SOUND_ID;

	FMOD::Channel* channelAssignedToSound = nullptr;
	m_fmodSystem->playSound( sound, nullptr, true, &channelAssignedToSound );
	if( channelAssignedToSound )
	{
		int loopCount = isLooped ? -1 : 0;
		unsigned int playbackMode = isLooped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		float frequency;
		channelAssignedToSound->setMode(playbackMode);
		channelAssignedToSound->getFrequency( &frequency );
		channelAssignedToSound->setFrequency( frequency * speed );
		channelAssignedToSound->setVolume( volume );
		channelAssignedToSound->setPan( balance );
		channelAssignedToSound->setLoopCount( loopCount );
	}

	channelAssignedToSound->set3DAttributes( (FMOD_VECTOR*) &position, (FMOD_VECTOR*) &velocity );
	channelAssignedToSound->setPaused( isPaused );

	return (SoundPlaybackID) channelAssignedToSound;
}


//----------------------------------------------------------------------------------------------------------------
void AudioSystem::SetSound3DParameters( SoundPlaybackID playbackID, const Vector3& position, const Vector3& velocity ) {
	FMOD::Channel* playbackChannel = (FMOD::Channel*) playbackID;
	playbackChannel->set3DAttributes((FMOD_VECTOR*) &position, (FMOD_VECTOR*) &velocity);
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::StopSound( SoundPlaybackID soundPlaybackID )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->stop();
}


//-----------------------------------------------------------------------------------------------
// Volume is in [0,1]
//
void AudioSystem::SetSoundPlaybackVolume( SoundPlaybackID soundPlaybackID, float volume )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set volume on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setVolume( volume );
}


//-----------------------------------------------------------------------------------------------
// Balance is in [-1,1], where 0 is L/R centered
//
void AudioSystem::SetSoundPlaybackBalance( SoundPlaybackID soundPlaybackID, float balance )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set balance on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	channelAssignedToSound->setPan( balance );
}


//-----------------------------------------------------------------------------------------------
// Speed is frequency multiplier (1.0 == normal)
//	A speed of 2.0 gives 2x frequency, i.e. exactly one octave higher
//	A speed of 0.5 gives 1/2 frequency, i.e. exactly one octave lower
//
void AudioSystem::SetSoundPlaybackSpeed( SoundPlaybackID soundPlaybackID, float speed )
{
	if( soundPlaybackID == MISSING_SOUND_ID )
	{
		ERROR_RECOVERABLE( "WARNING: attempt to set speed on missing sound playback ID!" );
		return;
	}

	FMOD::Channel* channelAssignedToSound = (FMOD::Channel*) soundPlaybackID;
	float frequency;
	FMOD::Sound* currentSound = nullptr;
	channelAssignedToSound->getCurrentSound( &currentSound );
	if( !currentSound )
		return;

	int ignored = 0;
	currentSound->getDefaults( &frequency, &ignored );
	channelAssignedToSound->setFrequency( frequency * speed );
}


//-----------------------------------------------------------------------------------------------
void AudioSystem::ValidateResult( FMOD_RESULT result )
{
	if( result != FMOD_OK )
	{
		ERROR_RECOVERABLE( Stringf( "Engine/Audio SYSTEM ERROR: Got error result code %i - error codes listed in fmod_common.h\n", (int) result ) );
	}
}


float AudioSystem::GetBassVolume() {
	if (m_fft->numchannels > 0) {
		return m_fft->spectrum[0][1];
	}
	else {
		return 0.f;
	}
}


//----------------------------------------------------------------------------------------------------------------
void AudioSystem::AddFFTToChannel( SoundPlaybackID sound ) {
	FMOD::Channel* channel = (FMOD::Channel*) sound;
	channel->addDSP(0, m_fftdsp);
}


//----------------------------------------------------------------------------------------------------------------
void AudioSystem::SetListenerParameters( const Vector3& position, const Vector3& velocity, const Vector3& forward, const Vector3& up ) {
	m_fmodSystem->set3DListenerAttributes( 0, (FMOD_VECTOR*) &position, (FMOD_VECTOR*) &velocity, (FMOD_VECTOR*) &forward, (FMOD_VECTOR*) &up );
}


//----------------------------------------------------------------------------------------------------------------
void AudioSystem::ActivateGlobalReverb() {
	FMOD_REVERB_PROPERTIES* reverbProperties = new FMOD_REVERB_PROPERTIES( FMOD_PRESET_STONECORRIDOR );
	m_fmodSystem->setReverbProperties( 0, reverbProperties );
	delete reverbProperties;
}


//----------------------------------------------------------------------------------------------------------------
void AudioSystem::DeactivateGlobalReverb() {
	FMOD_REVERB_PROPERTIES* reverbProperties = new FMOD_REVERB_PROPERTIES( FMOD_PRESET_OFF );
	m_fmodSystem->setReverbProperties( 0, reverbProperties );
	delete reverbProperties;
}

#endif // !defined( ENGINE_DISABLE_AUDIO )
