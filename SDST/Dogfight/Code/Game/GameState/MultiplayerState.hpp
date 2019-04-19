#pragma once
#include "Game/GameState/GameState.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerHUD.hpp"
#include "Game/Terrain.hpp"

#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Physics/Contacts.hpp"
#include "Engine/Net/NetSession.hpp"

#include <vector>
#include <map>


class MultiplayerState : public GameState {

public:
	MultiplayerState();
	//virtual void OnEnter() override;
	//virtual void OnBeginExit() override;
	//virtual void Update() override;
	//virtual void Render() override;

	void Initialize();

	void DestroyEntity( Entity* entity );
	Entity* GetEntityByID( int entityToFind );
	Entity* FindPlayerByConnection( uint8_t connectionIndex );
	bool DoesEntityExist( int entityToFind );
	int GetNextEntityToLockFromID( int lockingID, int prevID );

	float GetTimeSinceRoundStart() const;

	bool IsPointBelowTerrain( const Vector3& point );

public:
	PlayerInfo* m_winnerInfo = nullptr;
	Clock* m_sceneClock;
	Material* particleMaterial = nullptr;
	RenderSceneGraph* m_scene = nullptr;

	std::map< int, Entity* > entities;
	std::map< int, EntityController* > controllers;
	std::vector< ParticleEmitter* > m_orphanedParticleEmitters;
	EntityController* localPlayer = nullptr;
	FirstPersonCamera* m_camera = nullptr;
	PlayerHUD* hud = nullptr;

	int entityIdCounter = 0;
	bool m_debugDraw = false;


protected:
	virtual void SpawnPlayer() = 0;
	void ProcessPlayerInput();
	void UpdateEntitiesAndControllers();
	void CheckEntityCollisions();
	void ClearDeadEntities();

	void DrawScaleGridAroundPlayer();
	virtual void DrawEndScreen() = 0;

	bool AreOwnedBySamePlayer( Entity* first, Entity* second );


protected:

	NetSession* netSession = nullptr;
	std::string debugShaderName = "";

	CubeMap* m_skyboxTexture = nullptr;
	Shader* m_skyboxShader = nullptr;

	ForwardRenderPath* m_forwardRenderPath = nullptr;

	float ambientIntensity = 0.3f;
	Rgba ambientColor;

	Terrain* m_terrain = nullptr;
	Light* m_cameraLight = nullptr;
	Light* m_sun = nullptr;
	Vector3 lightPos = Vector3();
	Rgba lightColor;
	float specularPower = 1.f;
	float specularAmount = 0.2f;

	float m_matchStartTime = 0.f;

	Stopwatch* m_endScreenTimer = nullptr;
};