#pragma once
#include "Engine/Core/Image.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

#include <vector>
#include <string.h>

struct CampaignLevel {
public:
	std::string levelName;
	std::string pathToLevelImage;
};

class CampaignDefinition {
public:
	CampaignDefinition( const tinyxml2::XMLElement& xml );

	Image* GetLevelImage( unsigned int levelIndex );
	std::string GetLevelName( unsigned int levelIndex );

	static std::map<std::string, CampaignDefinition*> definitions;

private:

	std::string m_name;
	std::vector<CampaignLevel> m_levels;
};