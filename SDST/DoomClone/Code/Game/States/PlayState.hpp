#pragma once
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Physics/Contacts.hpp"

#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/States/GameState.hpp"
#include "Game/Map/GameMap.hpp"

#include <vector>


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


	void SignalPlayerDied();
	void SignalPlayerWins();


public:
	Clock* m_sceneClock;
	Material* particleMaterial = nullptr;
	SpriteSheet* terrain = nullptr;

	GameMap* testGameMap = nullptr;
	Entity* player = nullptr;
	ePlaySubstate currentSubstate = STATE_PLAYING;


private:


	CubeMap* m_skyboxTexture = nullptr;
	Shader* m_skyboxShader = nullptr;

	void ProcessPlayerInput();
	void ProcessInput();
	void ProcessDebugInput();
	void CheckIfPauseStateChanged();
	bool m_isPaused;

	//  [4/10/2018 Mitchel Pederson]


	Vector3 lightPos = Vector3();
	Rgba lightColor;


	std::string debugShaderName = "";

	Light* m_cameraLight = nullptr;
	Light* m_sun = nullptr;

	std::vector<Light*> m_pointLights;



};
