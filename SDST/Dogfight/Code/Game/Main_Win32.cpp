#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/Window.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/glbindings.h"


#define UNUSED(x) (void)(x);

App* g_theApp = nullptr;


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	commandLineString;

	g_theApp = new App();
	new Window(1280, 800, (void*) applicationInstanceHandle, "Protogame");
	g_theApp->Initialize();
	g_theApp->Run();

	delete g_theApp;
	return 0;
}


