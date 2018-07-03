#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Math/Vector2.hpp"
#include <vector>
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

#include "Game/Map/GameMap.hpp"




enum DevModeShader {
	DEV_SHADER_NONE,
	DEV_SHADER_VERTEX_NORMALS,
	DEV_SHADER_SURFACE_NORMALS,
	DEV_SHADER_WORLD_NORMALS,
	DEV_SHADER_TANGENTS,
	DEV_SHADER_BITANGENTS,
	DEV_SHADER_COLOR,
	DEV_SHADER_VERTEX_TANGENTS,
	DEV_SHADER_VERTEX_BITANGENTS,
	NUM_DEV_SHADERS
};

//-----------------------------------------------------------------------------------------------
// TheGame class created by Mitchel Pederson
// 
//


class TheGame {

public:
	TheGame();
	~TheGame();

	void Initialize();

	bool IsDevModeActive() const;
	void Update();
	void Render();

	float GetDeltaTime();
	float GetElapsedTime();
	Camera* GetPlayerCamera();
	
public:
	RenderSceneGraph* m_scene = nullptr;
	Clock* m_gameClock;
	Material* particleMaterial = nullptr;
	SpriteSheet* terrain = nullptr;
	GameMap* currentMap = nullptr;


private:
	void LoadTileDefinitions(std::string filePath);
	void LoadEntityDefinitions(std::string filePath);
	void LoadSpriteDefinitions(std::string filePath);
	void LoadIsoSpriteDefinitions(std::string filePath);
	void LoadIsoSpriteAnimDefinitions(std::string filePath);


private:

	SoundID music;
	SoundPlaybackID musicPlaybackID;

	void ProcessPlayerInput();

	Light* m_directional = nullptr;

	CubeMap* m_skyboxTexture = nullptr;
	Shader* m_skyboxShader = nullptr;

	// Constant stuff
	void ProcessInput();
	void ProcessDebugInput();
	void CheckIfPauseStateChanged();

	static TheGame* m_instance;

	bool m_devModeActive;
	DevModeShader m_devModeShader = DEV_SHADER_NONE;

	bool m_isPaused;

	Mesh* m_sphereMesh;
	Mesh* m_quadMesh;
	Mesh* m_cubeMesh;
	Mesh* m_basisMesh;
	Mesh* m_shipMesh;
	FirstPersonCamera* m_camera;

	ForwardRenderPath* m_forwardRenderPath = nullptr;


	//  [4/10/2018 Mitchel Pederson]
	float ambientIntensity = 0.08f;
	Rgba ambientColor;

	Vector3 lightPos = Vector3();
	Rgba lightColor;
	float specularPower = 25.f;
	float specularAmount = 0.3f;

	std::string debugShaderName = "";

	Light* m_cameraLight = nullptr;

	std::vector<Light*> m_pointLights;

};



