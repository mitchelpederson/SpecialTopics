#include "Engine/Audio/AudioCueDefinition.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"


std::map<std::string, AudioCueDefinition*> AudioCueDefinition::s_definitions;

AudioCueDefinition::AudioCueDefinition( const tinyxml2::XMLElement& xml ) {
	m_name = ParseXmlAttribute(xml, "name", m_name);

	const tinyxml2::XMLElement* soundElement = xml.FirstChildElement("soundfile");
	while (soundElement != nullptr) {

		std::string soundPath;
		soundPath = ParseXmlAttribute(*soundElement, "src", soundPath);

		SoundID sound = g_audioSystem->CreateOrGetSound(soundPath);
		m_sounds.push_back(sound);

		soundElement = soundElement->NextSiblingElement("soundfile");
	}

	s_definitions[m_name] = this;
}


SoundID AudioCueDefinition::GetRandomSoundFromCue() const {
	int soundIndex = GetRandomIntLessThan((int) m_sounds.size());
	return m_sounds[soundIndex];
}