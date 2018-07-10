#pragma once
#include "Game/Map/TileDefinition.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/FirstPersonCamera.hpp"
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

	void Update();
	void PushEntityOutOfTile( Entity* entity, IntVector2 const& tileCoords );

	void Render() const;
	void RenderMinimap() const;

	bool IsTileSolid( IntVector2 const& tileCoords ) const;
	bool IsTileVisible( IntVector2 tileCoords ) const;
	Camera* GetPlayerCamera();

	RaycastResult Raycast( Vector2 const& startPosition, float direction ) const;
	Entity* GetEntityAtPoint( Vector2 const& point ) const;

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

	std::vector<TileDefinition*> m_tiles;
	std::vector<Entity*> m_entities;
	IntVector2 m_dimensions;

	Mesh* m_minimap = nullptr;
	Renderable* m_minimapRenderable = nullptr;
	Camera* m_minimapCamera = nullptr;
	FirstPersonCamera* m_playerCamera = nullptr;

	ForwardRenderPath* m_forwardRenderPath = nullptr;

	// LIGHTING

	float specularPower = 100.f;
	float specularAmount = 0.1f;
	float ambientIntensity = 0.3f;
	Rgba ambientColor;

	// DEBUG
	bool m_shouldDisplayMinimap = true;
	bool m_isMinimapZoomed = false;
};