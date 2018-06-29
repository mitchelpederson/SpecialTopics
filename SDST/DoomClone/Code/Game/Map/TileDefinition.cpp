#include "Engine/Core/XmlUtilities.hpp"

#include "Game/Map/TileDefinition.hpp"


std::map<std::string, TileDefinition*> TileDefinition::s_definitions;

TileDefinition::TileDefinition( tinyxml2::XMLElement const& xml, SpriteSheet* textureSpriteSheet ) {

	tileSpriteLocation = textureSpriteSheet;

	name		= ParseXmlAttribute(xml, "name",		name);
	isSolid		= ParseXmlAttribute(xml, "isSolid",		isSolid);
	id			= ParseXmlAttribute(xml, "id",			id);
	isLevelExit = ParseXmlAttribute(xml, "isLevelExit", isLevelExit);
	minimapColor= ParseXmlAttribute(xml, "minimapColor",minimapColor);
	
	const tinyxml2::XMLElement* texturesElement = xml.FirstChildElement("textures");
	if (texturesElement != nullptr) {
		const tinyxml2::XMLElement* ceilingElement	= texturesElement->FirstChildElement("ceiling");
		const tinyxml2::XMLElement* wallElement		= texturesElement->FirstChildElement("wall");
		const tinyxml2::XMLElement* floorElement	= texturesElement->FirstChildElement("floor");

		if (ceilingElement != nullptr) {
			ceilingTexture = ParseXmlAttribute(*ceilingElement, "spriteCoord", ceilingTexture);
		}
		if (wallElement != nullptr) {
			wallTexture = ParseXmlAttribute(*wallElement, "spriteCoord", wallTexture);
		}
		if (floorElement != nullptr) {
			floorTexture = ParseXmlAttribute(*floorElement, "spriteCoord", floorTexture);
		}
	}
	s_definitions[name] = this;
}


TileDefinition* TileDefinition::GetTileDefByName( std::string const& name ) {
	std::map<std::string, TileDefinition*>::iterator it = s_definitions.find(name);
	return it->second;
}


TileDefinition* TileDefinition::GetTileByID( unsigned char id ) {
	std::map<std::string, TileDefinition*>::iterator it = s_definitions.begin();

	while (it != s_definitions.end()) {
		if (it->second->GetID() == id) {
			return it->second;
		}
		it++;
	}

	return nullptr;
}


Texture* TileDefinition::GetSpriteSheetTexture() const {
	return tileSpriteLocation->GetTexture();
}

AABB2 TileDefinition::GetCeilingUVs() const {
	return tileSpriteLocation->GetTexCoordsForSprteCoords(ceilingTexture);
}
AABB2 TileDefinition::GetWallUVs() const {
	return tileSpriteLocation->GetTexCoordsForSprteCoords(wallTexture);
}
AABB2 TileDefinition::GetFloorUVs() const {
	return tileSpriteLocation->GetTexCoordsForSprteCoords(floorTexture);
}

bool TileDefinition::IsSolid() const {
	return isSolid;
}
bool TileDefinition::IsLevelExit() const {
	return isLevelExit;
}

unsigned char TileDefinition::GetID() const {
	return id;
}


Rgba TileDefinition::GetMinimapColor() const {
	return minimapColor;
}