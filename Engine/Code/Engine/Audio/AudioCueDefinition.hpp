#pragma once

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

#include <map>
#include <vector>

class AudioCueDefinition {


public:
	AudioCueDefinition( const tinyxml2::XMLElement& xml );	
	
	static std::map<std::string, AudioCueDefinition*> s_definitions;

	SoundID GetRandomSoundFromCue() const;


private:
	std::string m_name;
	std::vector<SoundID> m_sounds;

};