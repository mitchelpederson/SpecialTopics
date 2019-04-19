#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/Window.hpp"
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
	m_mouseDelta = IntVector2();
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
	m_leftMouse.justReleased = false;
	m_middleMouse.justReleased = false;
	m_rightMouse.justReleased = false;
	m_leftMouse.justPressed = false;
	m_middleMouse.justPressed = false;
	m_rightMouse.justPressed = false;


	if ( m_isCursorLocked ) {
		::SetCursorPos( Window::GetInstance()->GetWidth() / 2, Window::GetInstance()->GetHeight() / 2 );
	}
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::SetCursorLock( bool isLocked ) {
	m_isCursorLocked = isLocked;
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::SetCursorVisibility( bool isVisible ) {
	m_isCursorVisible = isVisible;
	::ShowCursor(isVisible);

}


//-----------------------------------------------------------------------------------------------
void InputSystem::OnKeyPressed( unsigned char keyCode ) {
	m_keyStates[ keyCode ].isPressed = true;
	m_keyStates[ keyCode ].justPressed = true;
}


//-----------------------------------------------------------------------------------------------
void InputSystem::OnKeyReleased( unsigned char keyCode  ) {
	m_keyStates[ keyCode ].isPressed = false;
	m_keyStates[ keyCode ].justReleased = true;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyPressed( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].isPressed;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].justPressed;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const {
	return m_keyStates[ keyCode ].justReleased;
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::OnMouseMove( int lastX, int lastY ) {
	IntVector2 prevMouseLocation = m_mouseLocation;
	m_mouseLocation.x += lastX;
	m_mouseLocation.y += lastY;
	m_mouseDelta = m_mouseLocation - prevMouseLocation;
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::OnLeftMouseDown() {
	m_leftMouse.justPressed = true;
	m_leftMouse.isPressed = true;
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::OnLeftMouseUp() {
	m_leftMouse.justReleased = true;
	m_leftMouse.isPressed = false;
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::OnRightMouseDown() {
	m_rightMouse.justPressed = true;
	m_rightMouse.isPressed = true;
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::OnRightMouseUp() {
	m_rightMouse.justReleased = true;
	m_rightMouse.isPressed = false;
}

//----------------------------------------------------------------------------------------------------------------
void InputSystem::OnMiddleMouseDown() {
	m_middleMouse.justPressed = true;
	m_middleMouse.isPressed = true;
}


//----------------------------------------------------------------------------------------------------------------
void InputSystem::OnMiddleMouseUp() {
	m_middleMouse.justReleased = true;
	m_middleMouse.isPressed = false;
}


//----------------------------------------------------------------------------------------------------------------
IntVector2 InputSystem::GetRawMousePosition() const {
	return m_mouseLocation;
}


//----------------------------------------------------------------------------------------------------------------
IntVector2 InputSystem::GetMouseDelta() const {
	return m_mouseDelta;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::WasLeftMouseJustClicked() const {
	return m_leftMouse.justPressed ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::WasLeftMouseJustReleased() const {
	return m_leftMouse.justReleased ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::IsLeftMouseClicked() const {
	return m_leftMouse.isPressed ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::WasRightMouseJustClicked() const {
	return m_rightMouse.justPressed ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::WasRightMouseJustReleased() const {
	return m_rightMouse.justReleased ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::IsRightMouseClicked() const {
	return m_rightMouse.isPressed ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::WasMiddleMouseJustClicked() const {
	return m_middleMouse.justPressed ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::WasMiddleMouseJustReleased() const {
	return m_middleMouse.justReleased ? true : false;
}


//----------------------------------------------------------------------------------------------------------------
bool InputSystem::IsMiddleMouseClicked() const {
	return m_middleMouse.isPressed ? true : false;
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