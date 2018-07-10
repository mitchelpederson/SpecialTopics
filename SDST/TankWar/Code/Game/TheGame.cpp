#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/InputSystem/InputSystem.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"
#include "Game/states/LoadState.hpp"
#include "Game/States/MenuState.hpp"
#include "Game/States/PlayState.hpp"
#include "Game/States/SetupState.hpp"

typedef void (*command_cb)( const std::string& command ); 

TheGame* TheGame::m_instance = nullptr;
extern bool g_isQuitting;


TheGame::TheGame() { 

}


TheGame::~TheGame() {

}

void QuitGame( const std::string& command ) {
	command;
	g_isQuitting = true;
}


void TheGame::Initialize() {

	CommandRegistration::RegisterCommand("quit", QuitGame);
	
	m_state = new LoadState();
}


void TheGame::Update() {
	PROFILER_SCOPED_PUSH();
	m_state->Update();
	if (nextState != STATE_NONE && m_state->IsReadyToExit()) {
		GoToNextState();
	}

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F1)) {
		m_devModeActive = !m_devModeActive;
	}
}


void TheGame::Render() {
	PROFILER_SCOPED_PUSH();
	m_state->Render();
}


bool TheGame::IsDevModeActive() const {
	return m_devModeActive;
}


void TheGame::BeginTransitionToState( eGameState next ) {
	nextState = next;
	m_state->OnBeginExit();
}


void TheGame::GoToNextState() {

	delete m_state;

	if (nextState == STATE_MENU) {
		m_state = new MenuState();
	}
	else if (nextState == STATE_PLAY) {
		m_state = new PlayState();
	}
	else if (nextState == STATE_SETUP) {
		m_state = new SetupState();
	}
	currentState = nextState;
	nextState = STATE_NONE;

	m_state->OnEnter();
}


PlayState* TheGame::GetCurrentPlayState() {
	if (currentState == STATE_PLAY) {
		return (PlayState*) m_state;
	}
	else {
		return nullptr;
	}
}
