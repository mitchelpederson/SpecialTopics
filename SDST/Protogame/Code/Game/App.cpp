#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameDebug.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/InputSystem/InputSystem.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Blackboard.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerWindow.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Endianness.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Net/Net.hpp"



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


//----------------------------------------------------------------------------------------------------------------
void SwapEndianness( std::string const& command ) {
	Command c(command);
	int i = 0;

	if (c.GetNextInt(i)) {
		int j = i;
		ToEndianness(sizeof(int), &j, BIG_ENDIAN);
		DevConsole::Printf("little-endian: %d\nbig-endian: %d", i, j);
	} 
	else {
		DevConsole::Printf("Failed to read first int");
	}
}


//-----------------------------------------------------------------------------------------------
// Most basic loop of the game
//
void App::Run() {


	//----------------------------------------------------------------------------------------------------------------
	// Unit tests
	{
		size_t dat = 432151;
		uint16_t smallboy = 99;
		BytePacker bp;
		bp.WriteBytes(2, &smallboy);
		bp.WriteSize( dat );
		size_t dat2;
		size_t outboy;
		bp.ReadBytes(&outboy, 2);
		bp.ReadSize( &dat2 );

		size_t readSize;

		DevConsole::Printf("outboy: %u, ReadSize: %u", outboy, dat2);

		char const* str = "abcdefg";
		bp.WriteString(str);

		char* inStr = new char[10];
		bp.ReadString(inStr, 10);

		DevConsole::Printf("inStr: %s", inStr);
	}
	// Unit tests end
	//----------------------------------------------------------------------------------------------------------------

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

	CommandRegistration::SaveCommandHistory();
	DebugRenderShutdown();
	void (*fncptr)( unsigned int msg, size_t wparam, size_t lparam ) = GetMessages;
	Window::GetInstance()->UnregisterHandler(fncptr);
	Net::Shutdown();
}


//-----------------------------------------------------------------------------------------------
// Sets up the engine systems
//
void App::Initialize() {
	g_masterClock = new Clock();
	Logger::Startup();
	Net::Startup();
	g_theBlackboard = new Blackboard("Data/GameConfig.xml");
	g_theRenderer = new Renderer();
	g_theInputSystem = new InputSystem();
	g_audioSystem = new AudioSystem();
	g_theGame = new TheGame();
	Profiler::Initialize();
	g_theRenderer->Initialize();
	g_theGame->Initialize();

	DebugRenderStartup(g_theRenderer);
	RegisterDebugTimeCommands();

	void (*fncptr)( unsigned int msg, size_t wparam, size_t lparam ) = GetMessages;
	Window::GetInstance()->RegisterHandler(fncptr);

	new DevConsole();
	
	m_timeAtStartup = GetCurrentTimeSeconds();
	CommandRegistration::LoadCommandHistory();
	CommandRegistration::RegisterCommand("endianness", SwapEndianness, "Changes the integer from little to big endian and prints it as little endian");
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