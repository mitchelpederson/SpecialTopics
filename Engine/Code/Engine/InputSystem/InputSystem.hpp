#pragma once
#include "Engine/InputSystem/InputSystemCommon.hpp"
#include "Engine/InputSystem/XboxController.hpp"

class InputSystem {

public:
	InputSystem();
	~InputSystem();
	void BeginFrame();
	void EndFrame();

	void OnKeyPressed( unsigned char keyCode );
	void OnKeyReleased( unsigned char keyCode );
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
	static const int XBOX_DPAD_LEFT = 2;
	static const int XBOX_DPAD_RIGHT = 3;
	static const int XBOX_START = 4;
	static const int XBOX_BACK = 5;
	static const int XBOX_RB = 9;
	static const int XBOX_LB = 8;


private:
	KeyButtonState m_keyStates[ NUM_KEYS ];
	XboxController m_controllers[ NUM_CONTROLLERS ];

	void RunMessagePump();

};