#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

#include "Engine/Core/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

typedef void (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 

// For singleton style classes, I like to had the instance variable within the CPP; 
Window* Window::s_window = nullptr; // Instance Pointer; 
extern bool g_isQuitting;
extern HDC g_displayDeviceContext;


// I believe in App you will have a windows procedure you use that looks similar to the following; 
// This will be moved to Windows.cpp (here), with a slight tweak; 
LRESULT CALLBACK GameWndProc( HWND hwnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam )
{
	// NEW:  Give the custom handlers a chance to run first; 
	Window* window = Window::GetInstance(); 
	if (nullptr != window) {
		for (int i = 0; i < window->m_listeners.size(); ++i) {
			window->m_listeners[i]( msg, wParam, lParam ); 
		}
	}

	// Do what you were doing before - some of this may have to be moved; 
	switch (msg) {
			// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:		
		{
			break; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char) wParam;

			g_theInputSystem->OnKeyPressed(asKey);

			//return true;
			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wParam;

			g_theInputSystem->OnKeyReleased(asKey);

			//return true;
			break;
		}
	}

	// do default windows behaviour (return before this if you don't want default windows behaviour for certain messages)
	return ::DefWindowProc( hwnd, msg, wParam, lParam );
}


Window::Window( int width, int height, void* hinstance, const char* appName ) {
	HINSTANCE applicationInstanceHandle = (HINSTANCE) hinstance;

	s_window = this;

	m_width = width;
	m_height = height;
	m_aspect = (float) width / (float) height;

	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( GameWndProc ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = (float) m_width;
	float clientHeight = (float) m_height;


	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, appName, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	m_hwnd = (void*) CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( (HWND) m_hwnd, SW_SHOW );
	SetForegroundWindow( (HWND) m_hwnd );
	SetFocus( (HWND) m_hwnd );

	g_displayDeviceContext = GetDC( (HWND) m_hwnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	// Begin GL context stuff
	g_theRenderer->RenderStartup( m_hwnd );

}


Window* Window::GetInstance() {
	return s_window;
}


void Window::RegisterHandler( windows_message_handler_cb cb ) {
	m_listeners.push_back(cb);
}


void Window::UnregisterHandler( windows_message_handler_cb cb ) {

	std::vector<windows_message_handler_cb>::iterator it = m_listeners.begin();
	while (it != m_listeners.end()) {
		if (*it == cb) {
			m_listeners.erase(it);
			return;
		}
		it++;
	}
}


float Window::GetAspectRatio() {
	return (float) m_width / (float) m_height;
}