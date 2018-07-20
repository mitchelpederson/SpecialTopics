#pragma once
#include "Game/Map/TileDefinition.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"

#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"

#include <vector>


struct RaycastResult {
	bool hitEntity = false;
	bool hitWall = false;
	Vector2 location;
	Entity* entity = nullptr;
};

class GameMap {

public:
	GameMap( Image const& mapImage );
	~GameMap();

	void	Update();
	void	PushEntityOutOfTile( Entity* entity, IntVector2 const& tileCoords );
	void	SpawnPlayer( Player* playerFromPlayState );

	void	Render() const;
	void	RenderMinimap() const;

	bool	IsTileSolid( IntVector2 const& tileCoords ) const;
	bool	IsTileVisible( IntVector2 tileCoords ) const;
	bool	IsTileLevelExit( IntVector2 const& tileCoords ) const;
	Camera* GetPlayerCamera();

	RaycastResult	Raycast( Vector2 const& startPosition, float direction ) const;
	Entity*			GetEntityAtPoint( Vector2 const& point ) const;
	bool			IsPlayerOnVictoryTile();


public:
	Player* player = nullptr;
	RenderSceneGraph* scene = nullptr;


private:

	Mesh*	BuildMinimapMesh( Image const& image );

	void	AddTileMeshesToScene();
	void	SpawnEntities( Image const& mapImage );
	void	SpawnEntityFromIDOnTile( unsigned char id, IntVector2 const& coord );
	void	CorrectEntityCollisions();

	void	SortEntitiesByDistanceToPlayer();
	void	DeleteDeadEntities();

	void	SetClosestLightsToPoint( Vector3 const& point ) const;

	std::vector<TileDefinition*> m_tiles;
	std::vector<Entity*> m_entities;
	IntVector2 m_dimensions;
	Vector2 m_playerSpawn;

	Mesh* m_minimap = nullptr;
	Renderable* m_minimapRenderable = nullptr;
	Camera* m_minimapCamera = nullptr;
	FirstPersonCamera* m_playerCamera = nullptr;

	ForwardRenderPath* m_forwardRenderPath = nullptr;

	std::vector<Light*> m_lights;

	// LIGHTING

	float specularPower = 100.f;
	float specularAmount = 0.0f;
	float ambientIntensity = .3f;
	Rgba ambientColor;

	Light* m_playerLight = nullptr;

	// DEBUG
	bool m_shouldDisplayMinimap = false;
	bool m_isMinimapZoomed = false;
};