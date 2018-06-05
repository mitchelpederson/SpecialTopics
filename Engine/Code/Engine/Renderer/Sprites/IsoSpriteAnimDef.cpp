#include "Engine/Renderer/Sprites/IsoSpriteAnimDef.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


std::map<std::string, IsoSpriteAnimDef*> IsoSpriteAnimDef::s_definitions;

IsoSpriteAnimDef::IsoSpriteAnimDef( const tinyxml2::XMLElement& xml ) {

	std::string modeStr = "error";
	modeStr = ParseXmlAttribute(xml, "loop", modeStr);
	m_name = ParseXmlAttribute(xml, "id", m_name);

	if (modeStr == "clamp") {
		m_mode = ANIM_MODE_CLAMP;
	}
	else {
		m_mode = ANIM_MODE_LOOP;
	}

	const tinyxml2::XMLElement* frame = xml.FirstChildElement("frame");
	while ( frame != nullptr ) {
		FrameDef* currentFrame = new FrameDef();
		currentFrame->isoSprite = ParseXmlAttribute(*frame, "src", currentFrame->isoSprite);
		currentFrame->duration	= ParseXmlAttribute(*frame, "time", currentFrame->duration);
		m_duration += currentFrame->duration;
		m_frames.push_back(currentFrame);
		frame = frame->NextSiblingElement("frame");
	}

	s_definitions[m_name] = this;
}


IsoSprite* IsoSpriteAnimDef::GetIsoSpriteForTime( float timeIntoAnim ) const {

	float cumulativeDuration = 0.f;
	float timeIntoAnimReduced = timeIntoAnim; // for looping, keep subtracting duration until it's less than duration

	// If the mode is clamp and the time is past the duration, just get the last frame.
	if (m_mode == ANIM_MODE_CLAMP && m_duration < timeIntoAnim) {
		return m_frames[m_frames.size() - 1]->GetSprite();	
	}

	// make sure timeIntoAnimReduced is between 0 and m_duration
	while (timeIntoAnimReduced > m_duration) {
		timeIntoAnimReduced -= m_duration;
	}

	// now loop through the frames, adding their individual durations, until
	// their cumulative durations are greater than timeIntoAnimReduced
	for (int index = 0; index < m_frames.size(); index++) {
		cumulativeDuration += m_frames[index]->duration;
		if (cumulativeDuration > timeIntoAnimReduced) {
			return m_frames[index]->GetSprite();
		}
	}

	// if we return nullptr something went wrong with the def
	ERROR_AND_DIE("GetIsoSpriteForTime did not find a frame before it ran out of frames!");
}


float IsoSpriteAnimDef::GetDuration() const {
	return m_duration;
}


IsoSpriteAnimDef::eAnimMode IsoSpriteAnimDef::GetMode() const {
	return m_mode;
}


const std::string& IsoSpriteAnimDef::GetName() const {
	return m_name;
} 