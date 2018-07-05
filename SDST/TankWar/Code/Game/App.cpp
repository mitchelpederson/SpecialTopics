#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/InputSystem/InputSystem.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Blackboard.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerWindow.hpp"
#include "Game/GameDebug.hpp"

typedef void (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 


//-----------------------------------------------------------------------------------------------
// Global pointers to engine systems
//
Renderer* g_theRenderer;
TheGame* g_theGame;
InputSystem* g_theInputSystem;
Blackboard* g_theBlackboard;
Clock* g_masterClock;
AudioSystem* g_audioSystem;

bool g_isQuitting = false;

//-----------------------------------------------------------------------------------------------
// 
//
App::App() {

}


//-----------------------------------------------------------------------------------------------
// Get number of asteroids in play
//
App::~App() {

}


//-----------------------------------------------------------------------------------------------
// Most basic loop of the game
//
void App::Run() {

	while (!g_isQuitting) {

		g_masterClock->BeginFrame();
		Profiler::MarkFrame();
		g_theRenderer->BeginFrame();
		g_theInputSystem->BeginFrame();
		g_audioSystem->BeginFrame();
		DevConsole::GetInstance()->Update();
		ProfilerWindow::GetInstance()->Update();
		g_theGame->Update();
		g_theGame->Render();
		DebugRenderAndUpdate();
		ProfilerWindow::GetInstance()->Render();
		DevConsole::GetInstance()->Render();
		g_audioSystem->EndFrame();
		g_theInputSystem->EndFrame();
		g_theRenderer->EndFrame();

	}
	EngineShutdown();
	void (*fncptr)( unsigned int msg, size_t wparam, size_t lparam ) = GetMessages;
	Window::GetInstance()->UnregisterHandler(fncptr);
}


//-----------------------------------------------------------------------------------------------
// Sets up the engine systems
//
void App::Initialize() {
	
	EngineStartup();
	g_theGame = new TheGame();
	g_theGame->Initialize();


	RegisterDebugTimeCommands();

	void (*fncptr)( unsigned int msg, size_t wparam, size_t lparam ) = GetMessages;
	Window::GetInstance()->RegisterHandler(fncptr);

	m_timeAtStartup = GetCurrentTimeSeconds();
}


void App::GetMessages( unsigned int msg, size_t wParam, size_t lParam ) {
	wParam; lParam;
	switch (msg) {
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:		
	{
		g_isQuitting = true;
	}
	}
}