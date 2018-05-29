#pragma once 
#include "Engine/Math/Vector2.hpp"


class AnalogJoystick {
public:

	AnalogJoystick();

	void SetState( short x, short y );

	float GetCorrectedX() const;
	float GetCorrectedY() const;

	float GetAngle() const;
	float GetMagnitude() const;


private:
	Vector2 m_positionRawNormalized;
	Vector2 m_positionCorrectedNormalized;

	float m_angleDegrees;
	float m_magnitude;

	float m_minDeadZonePercent;
	float m_maxDeadZonePercent;

};