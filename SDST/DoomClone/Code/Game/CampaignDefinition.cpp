#include "Game/CampaignDefinition.hpp"
#include "Engine/Core/XmlUtilities.hpp"


std::map<std::string, CampaignDefinition*> CampaignDefinition::definitions;

//----------------------------------------------------------------------------------------------------------------
CampaignDefinition::CampaignDefinition( const tinyxml2::XMLElement& xml ) {
	m_name = ParseXmlAttribute( xml, "name", m_name );

	const tinyxml2::XMLElement* mapNode = xml.FirstChildElement("map");
	while ( mapNode != nullptr ) {

		CampaignLevel level;
		level.levelName			= ParseXmlAttribute( *mapNode, "name", level.levelName );
		level.pathToLevelImage	= ParseXmlAttribute( *mapNode, "src", level.pathToLevelImage );

		m_levels.push_back(level);
		mapNode = mapNode->NextSiblingElement("map");
	}

	definitions[m_name] = this;
}


//----------------------------------------------------------------------------------------------------------------
Image* CampaignDefinition::GetLevelImage( unsigned int levelIndex ) {
	std::string imageSrc = "Data/Maps/" + m_levels[levelIndex].pathToLevelImage;
	Image* levelImage = new Image(imageSrc);
	return levelImage;
}


//----------------------------------------------------------------------------------------------------------------
std::string CampaignDefinition::GetLevelName( unsigned int levelIndex ) {
	return m_levels[levelIndex].levelName;
}


//----------------------------------------------------------------------------------------------------------------
std::string CampaignDefinition::GetLevelPath( unsigned int levelIndex ) {
	return "Data/Maps/" + m_levels[levelIndex].pathToLevelImage;
}


//----------------------------------------------------------------------------------------------------------------
unsigned int CampaignDefinition::GetNumberOfLevels() {
	return (unsigned int) m_levels.size();
}

//----------------------------------------------------------------------------------------------------------------
std::string CampaignDefinition::GetCampaignName() {
	return m_name;
}