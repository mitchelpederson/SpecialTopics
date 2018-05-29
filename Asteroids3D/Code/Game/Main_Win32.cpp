#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Core/Window.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/glbindings.h"


#define UNUSED(x) (void)(x);

App* g_theApp = nullptr;


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{

	g_theApp = new App();
	new Window(1280, 800, (void*) applicationInstanceHandle);
	g_theApp->Initialize();
	g_theApp->Run();

	delete g_theApp;
	return 0;
}


