#pragma once
#include "Engine/Core/Image.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

#include <vector>
#include <map>
#include <string.h>


struct CampaignLevel {
	std::string levelName = "ERROR";
	std::string pathToLevelImage = "ERROR";
};


class CampaignDefinition {
public:
	CampaignDefinition( const tinyxml2::XMLElement& xml );

	Image* GetLevelImage( unsigned int levelIndex );
	std::string GetLevelName( unsigned int levelIndex );
	std::string GetLevelPath( unsigned int levelIndex );
	std::string GetCampaignName();
	unsigned int GetNumberOfLevels();

	static std::map<std::string, CampaignDefinition*> definitions;

private:

	std::string m_name;
	std::vector<CampaignLevel> m_levels;
};