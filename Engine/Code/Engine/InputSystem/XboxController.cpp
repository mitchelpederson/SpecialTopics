#include "Engine/Core/WindowsCommon.hpp"
#include <Xinput.h>
#pragma comment( lib, "xinput9_1_0" ) // Link in the xinput.lib static library // #Eiserloh: Xinput 1_4 doesn't work in Windows 7; use 9_1_0 explicitly for broadest compatibility
#include "Engine/Math/MathUtils.hpp"
#include "Engine/InputSystem/XboxController.hpp"


XboxController::XboxController() {
	for (int i = 0; i < 14; i++) {
		m_buttons[ i ].isPressed = false;
		m_buttons[ i ].justPressed = false;
		m_buttons[ i ].justReleased = false;
	}
}

XboxController::~XboxController() {

}


void XboxController::Update() {

		// Next four lines copied from sample code, modified controllerID variable
	XINPUT_STATE xboxControllerState;
	memset( &xboxControllerState, 0, sizeof( xboxControllerState ) );
	DWORD errorStatus = XInputGetState( m_controllerID, &xboxControllerState );

	if( errorStatus == ERROR_SUCCESS ) {

		m_isConnected = true;

		XINPUT_GAMEPAD& gamepad = xboxControllerState.Gamepad;

			// Get triggers and normalize
		m_leftTriggerNormalized = (float) gamepad.bLeftTrigger / 255.f;
		m_rightTriggerNormalized = (float) gamepad.bRightTrigger / 255.f;

			// Get sticks
		m_leftJoystick.SetState(gamepad.sThumbLX, gamepad.sThumbLY);
		m_rightJoystick.SetState(gamepad.sThumbRX, gamepad.sThumbRY);

		UpdateButton(0, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP));
		UpdateButton(1, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN));
		UpdateButton(2, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT));
		UpdateButton(3, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT));

		UpdateButton(4, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_START));
		UpdateButton(5, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_BACK));
		UpdateButton(6, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB));
		UpdateButton(7, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB));

		UpdateButton(8, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER));
		UpdateButton(9, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER));
		UpdateButton(10, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_A));
		UpdateButton(11, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_B));
		UpdateButton(12, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_X));
		UpdateButton(13, IsBitSet(gamepad.wButtons, XINPUT_GAMEPAD_Y));

	}
	else {
		m_isConnected = false;
	}
}


void XboxController::UpdateButton(int button, bool isPressed) {

	m_buttons[ button ].justPressed = false;
	m_buttons[ button ].justReleased = false;
		
		// If the button is now pressed but wasn't last frame, set justPressed to true
	if ( isPressed == true && m_buttons[ button ].isPressed == false ) {
		m_buttons[ button ].justPressed = true;
	}

		// If it's not pressed now but was last frame, set justReleased to true
	else if (isPressed == false && m_buttons[ button ].isPressed == true ) {
		m_buttons[ button ].justReleased = true;
	}

	m_buttons[ button ].isPressed = isPressed;

}


void XboxController::SetId(int id) {
	m_controllerID = id;
}


bool XboxController::IsButtonPressed( int button ) const {
	return m_buttons[ button ].isPressed;
}

bool XboxController::WasButtonJustPressed( int button ) const {
	return m_buttons[ button ].justPressed;
}

bool XboxController::WasButtonJustReleased( int button ) const {
	return m_buttons[ button ].justReleased;
}

float XboxController::GetLeftStickX() const {
	return m_leftJoystick.GetCorrectedX();
}

float XboxController::GetLeftStickY() const {
	return m_leftJoystick.GetCorrectedY();
}

float XboxController::GetRightStickX() const {
	return m_rightJoystick.GetCorrectedX();
}

float XboxController::GetRightStickY() const {
	return m_rightJoystick.GetCorrectedY();
}

float XboxController::GetLeftStickAngle() const {
	return m_leftJoystick.GetAngle();
}

float XboxController::GetLeftStickMagnitude() const {
	return m_leftJoystick.GetMagnitude();
}

float XboxController::GetRightStickAngle() const {
	return m_rightJoystick.GetAngle();
}

float XboxController::GetRightStickMagnitude() const {
	return m_rightJoystick.GetMagnitude();
}

float XboxController::GetLeftTrigger() const {
	return m_leftTriggerNormalized;
}

float XboxController::GetRightTrigger() const {
	return m_rightTriggerNormalized;
}

bool XboxController::IsConnected() const {
	return m_isConnected;
}