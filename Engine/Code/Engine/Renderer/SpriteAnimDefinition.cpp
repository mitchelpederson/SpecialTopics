#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <string.h>
#include <vector>

SpriteAnimDefinition::SpriteAnimDefinition( const tinyxml2::XMLElement& xml, int defaultFPS ) {


	// Get the sprite indexes from the attribute spriteIndexes
	std::string framesString = "ERROR";
	framesString = ParseXmlAttribute(xml, "spriteIndexes", framesString);
	GUARANTEE_OR_DIE(framesString != "ERROR", "SpriteAnim did not have a valid sspriteIndexes");

	// Convert the tokenized index string into ints
	std::vector<std::string> framesStringSplit = SplitString(framesString, ',');
	for (int token = 0; token < framesStringSplit.size(); token++) {
		int index = atoi( framesStringSplit[ token ].c_str() );
		m_frameIndices.push_back( index );
	}
	GUARANTEE_OR_DIE(m_frameIndices.size() >= 1, "There are no indices for an animation!");

	m_fps		= ParseXmlAttribute(xml, "fps", defaultFPS);
	GUARANTEE_OR_DIE(m_fps > 0, "FPS not set for an animation!");
	m_isLooping = ParseXmlAttribute(xml, "isLooping", m_isLooping);

	m_durationSeconds =  ((float) m_frameIndices.size() / (float) m_fps);
}


int SpriteAnimDefinition::GetNumOfFrames() const {
	return (int) m_frameIndices.size();
}


SpriteSheet* SpriteAnimDefinition::GetSpriteSheet() const {
	return m_spriteSheet;
}


