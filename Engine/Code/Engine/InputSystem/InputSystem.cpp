
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places

#include "Engine/InputSystem/InputSystem.hpp"


//-----------------------------------------------------------------------------------------------
// 
//
InputSystem::InputSystem() {
	for (int i = 0; i < NUM_CONTROLLERS; i++) {
		m_controllers[ i ].SetId(i);
	}
}

//-----------------------------------------------------------------------------------------------
// 
//
InputSystem::~InputSystem() {

}


//-----------------------------------------------------------------------------------------------
// Process windows message events and poll each controller
//
void InputSystem::BeginFrame() {
	RunMessagePump();

	for (int i = 0; i < NUM_CONTROLLERS; i++) {
		m_controllers[ i ].Update();
	}
}


//-----------------------------------------------------------------------------------------------
// Reset all justPressed and justReleased values for the next frame
//
void InputSystem::EndFrame() {
	for (int i = 0; i < NUM_KEYS; i++) {
		m_keyStates[ i ].justPressed = false;
		m_keyStates[ i ].justReleased = false;

	}
}


//-----------------------------------------------------------------------------------------------
// 
//
void InputSystem::OnKeyPressed( unsigned char keyCode ) {
	m_keyStates[ keyCode ].isPressed = true;
	m_keyStates[ keyCode ].justPressed = true;
}


//-----------------------------------------------------------------------------------------------
// 
//
void InputSystem::OnKeyReleased( unsigned char keyCode  ) {
	m_keyStates[ keyCode ].isPressed = false;
	m_keyStates[ keyCode ].justReleased = true;
}


//-----------------------------------------------------------------------------------------------
// 
//
bool InputSystem::IsKeyPressed( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].isPressed;
}


//-----------------------------------------------------------------------------------------------
// 
//
bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].justPressed;
}


//-----------------------------------------------------------------------------------------------
// 
//
bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].justReleased;
}


//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
void InputSystem::RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" function
	}
}


//-----------------------------------------------------------------------------------------------
// 
//
const XboxController& InputSystem::GetController(int id) const {
	const XboxController& controller = m_controllers[ id ];
	return controller;
}