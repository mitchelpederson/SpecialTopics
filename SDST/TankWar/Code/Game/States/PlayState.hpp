#pragma once
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Physics/Contacts.hpp"

#include "Game/GameObject.hpp"
#include "Game/Tank.hpp"
#include "Game/Bullet.hpp"
#include "Game/SwarmEnemy.hpp"
#include "Game/Base.hpp"
#include "Game/States/GameState.hpp"
#include "Game/Map/GameMap.hpp"

#include <vector>

struct RaycastResult {
	RaycastHit3 contact;
	bool isTerrain;
	bool isGameObject;
};

enum ePlaySubstate {
	STATE_PLAYING,
	STATE_PLAYER_DIED,
	STATE_PLAYER_WINS
};

class PlayState : public GameState {

public:
	PlayState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() const override;

	void Initialize();

	void SpawnBaseInRandomSpot();
	SwarmEnemy* SpawnSwarmEnemyAtSpot( const Vector3& position, Base* parent );
	void SpawnBullet( const Vector3& position, const Vector3& forward, float speed, eTeam team );
	static void SpawnDebugSphereOverPlayer( const std::string& command );

	void SignalPlayerDied();
	void SignalPlayerWins();

	void RemoveBase( Base* base );
	void RemoveSwarmEnemy( SwarmEnemy* enemy );
	void RemoveBullet( Bullet* bullet );

	RaycastHit3 Raycast( unsigned int maxContacts, const Ray3& ray );
	std::vector<SwarmEnemy*> GetSwarmersInRadius( const Vector3& point, float radius );

	
public:
	RenderSceneGraph* m_scene = nullptr;
	Clock* m_sceneClock;
	Material* particleMaterial = nullptr;
	SpriteSheet* terrain = nullptr;
	OrbitCamera* m_camera;


	GameMap* testGameMap = nullptr;
	GameObject* player = nullptr;
	ePlaySubstate currentSubstate = STATE_PLAYING;


private:

	void CheckForCombatCollisions();
	void CheckForPlayerVictory();
	void ClearDeadGameObjects();

	void RenderUI() const;

	int m_enemyCount = 0;

	std::vector<GameObject*> m_sceneObjects;
	std::vector<Bullet*> m_bullets;
	std::vector<Base*> m_bases;
	std::vector<SwarmEnemy*> m_swarmers;
	
	Stopwatch m_playerRespawnTimer;

	SoundPlaybackID m_bgMusic;

	Light* m_directional = nullptr;

	CubeMap* m_skyboxTexture = nullptr;
	Shader* m_skyboxShader = nullptr;

	void ProcessPlayerInput();
	void ProcessInput();
	void ProcessDebugInput();
	void CheckIfPauseStateChanged();
	bool m_isPaused;


	Mesh* m_sphereMesh;
	Mesh* m_quadMesh;
	Mesh* m_cubeMesh;
	Mesh* m_basisMesh;

	ForwardRenderPath* m_forwardRenderPath = nullptr;

	//  [4/10/2018 Mitchel Pederson]
	float ambientIntensity = 0.3f;
	Rgba ambientColor;

	Vector3 lightPos = Vector3();
	Rgba lightColor;
	float specularPower = 100.f;
	float specularAmount = 0.1f;

	std::string debugShaderName = "";

	Light* m_cameraLight = nullptr;
	Light* m_sun = nullptr;

	std::vector<Light*> m_pointLights;



};
