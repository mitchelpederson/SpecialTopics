#pragma once
#include "Game/Entity.hpp"
#include "Game/GameState/GameState.hpp"

#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Physics/Contacts.hpp"
#include "Engine/Net/NetSession.hpp"

#include <vector>


enum ePlaySubstate {
	STATE_PLAYING,
	STATE_PLAYER_DIED,
	STATE_LEVEL_COMPLETE,
	STATE_CAMPAIGN_COMPLETE
};

class PlayState : public GameState {

public:
	PlayState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;

	void Initialize();


public:
	Clock* m_sceneClock;
	Material* particleMaterial = nullptr;
	SpriteSheet* terrain = nullptr;

	ePlaySubstate currentSubstate = STATE_PLAYING;


private:
	Stopwatch m_moveToNextLevelTimer;
	unsigned int currentLevelInCampaign = 0;

	SoundPlaybackID m_backgroundMusic;


	void ProcessPlayerInput();
	void ProcessInput();
	void ProcessDebugInput();
	void CheckIfPauseStateChanged();
	void LoadAndStartLevel( unsigned int levelIndex );
	bool m_isPaused;

	std::string debugShaderName = "";


	CubeMap* m_skyboxTexture = nullptr;
	Shader* m_skyboxShader = nullptr;
	Vector3 lightPos = Vector3();
	Rgba lightColor;
	Light* m_cameraLight = nullptr;
	Light* m_sun = nullptr;

	std::vector<Light*> m_pointLights;
};
