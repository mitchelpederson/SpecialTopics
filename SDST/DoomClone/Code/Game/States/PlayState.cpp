#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"

#include "Game/States/PlayState.hpp"
#include "Game/GameCommon.hpp"


PlayState::PlayState()
	: m_sceneClock(new Clock(g_masterClock))
{
}


void PlayState::Initialize() {

	particleMaterial = g_theRenderer->GetMaterial("additive");

	Image gameMapImage("Data/Maps/test4.png");
	testGameMap = new GameMap(gameMapImage);

}

void PlayState::OnEnter() {
	BeginFadeIn(0.5f);
	Initialize();
}

void PlayState::OnBeginExit() {
	BeginFadeOut(0.5f);
}

//-----------------------------------------------------------------------------------------------
// Process input relating to game logic EXCEPT pausing and time scaling
//
void PlayState::ProcessInput() {
	ProcessDebugInput();
	ProcessPlayerInput();
}


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

void PlayState::ProcessDebugInput() {

}


void PlayState::Update() {
	PROFILER_SCOPED_PUSH();

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) && !IsFading() && !DevConsole::GetInstance()->IsOpen()) {
		g_theGame->BeginTransitionToState(STATE_MENU);
	}
	GameState::Update();

	CheckIfPauseStateChanged();

	if (!DevConsole::GetInstance()->IsOpen()) {
		ProcessInput();
	}

	testGameMap->Update();

	
	
}


void PlayState::Render() const {
	PROFILER_SCOPED_PUSH();
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("passthrough"));
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	GameState::Render();

	// Begin actual game render


	testGameMap->Render();

	g_theRenderer->SetShader(nullptr);
	g_theRenderer->DisableDepth();
	g_theRenderer->SetCameraToUI();

	if (currentSubstate != STATE_PLAYING) {
		g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, 100));
	}

	if (currentSubstate == STATE_PLAYER_DIED) {
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 50.f, 100.f, 100.f), Vector2(0.5f, 0.f), "YOU DIED", 20.f, Rgba(200, 0, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);	
	}

	if (currentSubstate == STATE_PLAYER_WINS) {
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 50.f, 100.f, 100.f), Vector2(0.5f, 0.f), "You win!", 20.f, Rgba(0, 200, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 50.f), Vector2(0.5f, 1.f), "Press Enter to return to the main menu", 6.f, Rgba(0, 200, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);

	}


	GameState::Render();
}


void PlayState::CheckIfPauseStateChanged() {
	if (!DevConsole::GetInstance()->IsOpen() && g_theInputSystem->WasKeyJustPressed('P') && m_isPaused == false) {
		m_sceneClock->SetPaused(true);
	}
	else {
		m_sceneClock->SetPaused(false);
	}
}


void PlayState::SignalPlayerDied() {
	currentSubstate = STATE_PLAYER_DIED;
}
