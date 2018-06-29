#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include <map>

class TileDefinition {

public:
	TileDefinition( tinyxml2::XMLElement const& xml, SpriteSheet* textureSpriteSheet );

	Texture* GetSpriteSheetTexture() const;
	bool IsSolid() const;
	bool IsLevelExit() const;
	AABB2 GetCeilingUVs() const;
	AABB2 GetFloorUVs() const;
	AABB2 GetWallUVs() const;
	unsigned char GetID() const;
	Rgba GetMinimapColor() const;

public:
	static std::map<std::string, TileDefinition*> s_definitions;

	static TileDefinition* GetTileDefByName( std::string const& name );
	static TileDefinition* GetTileByID( unsigned char id );


private:
	std::string name = "";
	unsigned char id = 0;
	IntVector2 ceilingTexture;
	IntVector2 wallTexture;
	IntVector2 floorTexture;
	SpriteSheet* tileSpriteLocation = nullptr;
	Rgba minimapColor = Rgba();

	bool isSolid = false;
	bool isLevelExit = false;


};