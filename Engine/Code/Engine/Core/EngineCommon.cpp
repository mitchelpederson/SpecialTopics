#include "Engine/Core/EngineCommon.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerWindow.hpp"
#include "Engine/Renderer/DebugRender.hpp"

void EngineStartup() {
	g_masterClock = new Clock();
	g_theBlackboard = new Blackboard("Data/GameConfig.xml");
	g_theRenderer = new Renderer();
	g_theInputSystem = new InputSystem();
	g_audioSystem = new AudioSystem();
	g_theRenderer->Initialize();
	Profiler::Initialize();
	ProfilerWindow::Initialize();
	DebugRenderStartup(g_theRenderer);
	new DevConsole();
}


void EngineShutdown() {
	DebugRenderShutdown();
}