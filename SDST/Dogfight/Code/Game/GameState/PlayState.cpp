#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"

#include "Game/GameState/PlayState.hpp"
#include "Game/GameCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
PlayState::PlayState()
	: m_sceneClock(new Clock(g_masterClock))
	, m_moveToNextLevelTimer(m_sceneClock)
{
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::Initialize() {

	particleMaterial = g_theRenderer->GetMaterial("additive");

}


//----------------------------------------------------------------------------------------------------------------
void PlayState::OnEnter() {
	BeginFadeIn(0.5f);
	Initialize();
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::OnBeginExit() {
	BeginFadeOut(0.5f);
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::ProcessInput() {
	ProcessDebugInput();
	ProcessPlayerInput();
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::ProcessPlayerInput() {
	PROFILER_SCOPED_PUSH();
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::ProcessDebugInput() {

}


//----------------------------------------------------------------------------------------------------------------
void PlayState::Update() {
	PROFILER_SCOPED_PUSH();

	if (currentSubstate == STATE_PLAYING) {

	}


	GameState::Update();

	CheckIfPauseStateChanged();	
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::Render() {
	PROFILER_SCOPED_PUSH();
	

	// Render the game and minimap

	// Render fade in/out before state messages
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("passthrough"));
	GameState::Render();

	// Render any state related UI like victory or death message
	g_theRenderer->SetShader(nullptr);
	g_theRenderer->DisableDepth();
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));

	if (currentSubstate != STATE_PLAYING) {
	}

	if (currentSubstate == STATE_PLAYER_DIED) {
	}

	if (currentSubstate == STATE_LEVEL_COMPLETE) {
	}

	if (currentSubstate == STATE_CAMPAIGN_COMPLETE) {
	}

}


//----------------------------------------------------------------------------------------------------------------
void PlayState::CheckIfPauseStateChanged() {
	if (!DevConsole::GetInstance()->IsOpen() && g_theInputSystem->WasKeyJustPressed('P') && m_isPaused == false) {
		m_sceneClock->SetPaused(true);
	}
	else {
		m_sceneClock->SetPaused(false);
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::LoadAndStartLevel( unsigned int levelIndex ) {

	BeginFadeIn(1.f);
}