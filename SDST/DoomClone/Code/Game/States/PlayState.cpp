#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"

#include "Game/States/PlayState.hpp"
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

	player = new Player();
	LoadAndStartLevel(0);

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
	

	if (currentSubstate == STATE_PLAYER_DIED) {
		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_SPACE)) {
			delete player;
			player = new Player();
			currentSubstate = STATE_PLAYING;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::ProcessDebugInput() {

}


//----------------------------------------------------------------------------------------------------------------
void PlayState::Update() {
	PROFILER_SCOPED_PUSH();

	if (currentSubstate == STATE_PLAYING) {

		if (testGameMap->IsPlayerOnVictoryTile()) {
			currentSubstate = STATE_LEVEL_COMPLETE;
			m_moveToNextLevelTimer.SetTimer(1.f);
			
			if (currentLevelInCampaign != g_theGame->currentCampaign->GetNumberOfLevels()) {
				BeginFadeOut(1.f);
			}
		}

		if (!DevConsole::GetInstance()->IsOpen()) {
			ProcessInput();
		}
	}

	if (currentSubstate == STATE_LEVEL_COMPLETE) { 
		if (m_moveToNextLevelTimer.HasElapsed()) {
			GoToNextLevel();
		}
	}

	if (currentSubstate == STATE_CAMPAIGN_COMPLETE) {
		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) &&  !DevConsole::GetInstance()->IsOpen()) {
			g_theGame->BeginTransitionToState(STATE_MENU);
		}
	}

	GameState::Update();

	CheckIfPauseStateChanged();

	testGameMap->Update();
	
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::Render() const {
	PROFILER_SCOPED_PUSH();
	

	// Render the game and minimap
	testGameMap->Render();

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
		g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, 100));
	}

	if (currentSubstate == STATE_PLAYER_DIED) {
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 50.f, 100.f, 100.f), Vector2(0.5f, 0.f), "YOU DIED", 20.f, Rgba(200, 0, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_WORD_WRAP);	
	}

	if (currentSubstate == STATE_LEVEL_COMPLETE) {
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 50.f, 100.f, 100.f), Vector2(0.5f, 0.f), "LEVEL COMPLETE", 20.f, Rgba(0, 200, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_WORD_WRAP);
	}

	if (currentSubstate == STATE_CAMPAIGN_COMPLETE) {
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 50.f, 100.f, 100.f), Vector2(0.5f, 0.f), "You win this campaign!", 20.f, Rgba(0, 200, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_WORD_WRAP);
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 50.f), Vector2(0.5f, 1.f), "Press Enter to return to the main menu", 6.f, Rgba(0, 200, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_WORD_WRAP);
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
void PlayState::SignalPlayerDied() {
	currentSubstate = STATE_PLAYER_DIED;
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::GoToNextLevel() {
	currentLevelInCampaign++;
	if (currentLevelInCampaign == g_theGame->currentCampaign->GetNumberOfLevels()) {
		currentSubstate = STATE_CAMPAIGN_COMPLETE;
	}

	else {
		LoadAndStartLevel(currentLevelInCampaign);
		currentSubstate = STATE_PLAYING;
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayState::LoadAndStartLevel( unsigned int levelIndex ) {
	delete testGameMap;
	Image gameMapImage( g_theGame->currentCampaign->GetLevelPath(levelIndex) );
	testGameMap = new GameMap( gameMapImage );
	testGameMap->SpawnPlayer( player );
	BeginFadeIn(1.f);
}