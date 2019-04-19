#pragma once
#include "Engine/InputSystem/InputSystemCommon.hpp"
#include "Engine/InputSystem/XboxController.hpp"
#include "Engine/Math/IntVector2.hpp"

class InputSystem {

public:
	InputSystem();
	~InputSystem();
	void BeginFrame();
	void EndFrame();

	void SetCursorVisibility( bool isVisible );
	void SetCursorLock( bool isLocked );
	
	void OnMouseMove( int lastX, int lastY );
	void OnLeftMouseDown();
	void OnRightMouseDown();
	void OnMiddleMouseDown();
	void OnLeftMouseUp();
	void OnRightMouseUp();
	void OnMiddleMouseUp();

	void OnKeyPressed( unsigned char keyCode );
	void OnKeyReleased( unsigned char keyCode );

	IntVector2 GetRawMousePosition() const;
	IntVector2 GetMouseDelta() const;
	bool WasLeftMouseJustClicked() const;
	bool WasLeftMouseJustReleased() const;
	bool IsLeftMouseClicked() const;
	bool WasRightMouseJustClicked() const;
	bool WasRightMouseJustReleased() const;
	bool IsRightMouseClicked() const;
	bool WasMiddleMouseJustClicked() const;
	bool WasMiddleMouseJustReleased() const;
	bool IsMiddleMouseClicked() const;

	bool IsKeyPressed( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

	const XboxController& GetController( int id ) const;

	
	static const int NUM_KEYS = 256;
	static const int NUM_CONTROLLERS = 4;
	static const int KEYBOARD_F1 = 0x70;
	static const int KEYBOARD_F2 = 0x71;
	static const int KEYBOARD_F3 = 0x72;
	static const int KEYBOARD_F4 = 0x73;
	static const int KEYBOARD_F5 = 0x74;
	static const int KEYBOARD_F6 = 0x75;
	static const int KEYBOARD_F7 = 0x76;
	static const int KEYBOARD_F8 = 0x77;
	static const int KEYBOARD_F9 = 0x78;
	static const int KEYBOARD_F10 = 0x79;
	static const int KEYBOARD_F11 = 0x7A;
	static const int KEYBOARD_SPACE = 0x20;
	static const int KEYBOARD_ESC = 0x1B;
	static const int KEYBOARD_TILDE = 0xC0;
	static const int KEYBOARD_ENTER = 0x0D;
	static const int KEYBOARD_SHIFT = 0x10;
	static const int KEYBOARD_LEFT = 0x25;
	static const int KEYBOARD_RIGHT = 0x27;
	static const int KEYBOARD_UP = 0x26;
	static const int KEYBOARD_DOWN = 0x28;
	static const int KEYBOARD_PERIOD = 0xBE;
	static const int KEYBOARD_DELETE = 0x2E;
	static const int KEYBOARD_BACKSPACE = 0x08;
	static const int KEYBOARD_OPENBRACKET = 0xDB;
	static const int KEYBOARD_CLOSEBRACKET = 0xDD;
	static const int KEYBOARD_BACKSLASH = 0xDC;
	static const int KEYBOARD_SEMICOLON = 0xBA;
	static const int KEYBOARD_APOSTROPHE = 0xDE;
	static const int KEYBOARD_COMMA = 0xBC;
	static const int KEYBOARD_FORWARDSLASH = 0xBF;
	static const int KEYBOARD_DASH = 0xBD;
	static const int KEYBOARD_EQUALS = 0xBB;

	static const int XBOX_A = 10;
	static const int XBOX_B = 11;
	static const int XBOX_X = 12;
	static const int XBOX_Y = 13;
	static const int XBOX_DPAD_LEFT = 2;
	static const int XBOX_DPAD_RIGHT = 3;
	static const int XBOX_DPAD_UP = 0;
	static const int XBOX_DPAD_DOWN = 1;
	static const int XBOX_START = 4;
	static const int XBOX_BACK = 5;
	static const int XBOX_RB = 9;
	static const int XBOX_LB = 8;
	static const int XBOX_LS = 6;
	static const int XBOX_RS = 7;


private:
	KeyButtonState m_keyStates[ NUM_KEYS ];
	XboxController m_controllers[ NUM_CONTROLLERS ];
	IntVector2 m_mouseLocation;
	IntVector2 m_mouseDelta;
	KeyButtonState m_leftMouse;
	KeyButtonState m_middleMouse;
	KeyButtonState m_rightMouse;

	void RunMessagePump();

	bool m_isCursorVisible = true;
	bool m_isCursorLocked = false;
};