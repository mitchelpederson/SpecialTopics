#pragma once
#include "Engine/InputSystem/InputSystemCommon.hpp"
#include "Engine/InputSystem/AnalogJoystick.hpp"


class XboxController {

public:
	XboxController();
	~XboxController();

	void Update();

	void SetId(int id);
	bool IsConnected() const;

	bool IsButtonPressed(int button) const;
	bool WasButtonJustPressed(int button) const;
	bool WasButtonJustReleased(int button) const;

	float GetLeftStickX() const;
	float GetLeftStickY() const;
	float GetRightStickX() const;
	float GetRightStickY() const;

	float GetLeftStickAngle() const;
	float GetLeftStickMagnitude() const;
	float GetRightStickAngle() const;
	float GetRightStickMagnitude() const;

	float GetLeftTrigger() const;
	float GetRightTrigger() const;

private:
	KeyButtonState m_buttons[ 14 ];

	AnalogJoystick m_leftJoystick;
	AnalogJoystick m_rightJoystick;

	float m_leftTriggerNormalized;
	float m_rightTriggerNormalized;

	int m_controllerID;
	bool m_isConnected = false;

	void UpdateButton(int button, bool isPressed);


};