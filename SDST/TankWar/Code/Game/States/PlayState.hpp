#pragma once
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"

#include "Game/GameObject.hpp"
#include "Game/States/GameState.hpp"
#include "Game/Map/GameMap.hpp"

#include <vector>


class PlayState : public GameState {

public:
	PlayState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() const override;

	void Initialize();

	static void SpawnDebugSphereOverPlayer( const std::string& command );

	
public:
	RenderSceneGraph* m_scene = nullptr;
	Clock* m_sceneClock;
	Material* particleMaterial = nullptr;
	SpriteSheet* terrain = nullptr;

	GameMap* testGameMap = nullptr;
	GameObject* player = nullptr;

private:

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
	FirstPersonCamera* m_camera;

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
