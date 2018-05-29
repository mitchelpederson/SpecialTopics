#include "Engine/InputSystem/AnalogJoystick.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>



AnalogJoystick::AnalogJoystick() : m_minDeadZonePercent(0.16f), m_maxDeadZonePercent(0.95f) {

}


//-----------------------------------------------------------------------------------------------
// Generates the normalized cartesian and polar coordinate position of the stick
//
void AnalogJoystick::SetState(short x, short y) {

	float rawMax = 32767.f;

	m_positionRawNormalized = Vector2( ClampFloat((float) x / rawMax, -1.f, 1.f), 
									ClampFloat((float) y / rawMax, -1.f, 1.f));

		// Get the polar coordinates
	m_angleDegrees = m_positionRawNormalized.GetOrientationDegrees();
	m_magnitude = m_positionRawNormalized.GetLength();

	if (m_magnitude < m_minDeadZonePercent) {
		m_magnitude = 0.f;
	}

	else if (m_magnitude > m_maxDeadZonePercent) {
		m_magnitude = 1.f;
	}

	else {
		m_magnitude = RangeMapFloat( m_magnitude, m_minDeadZonePercent, m_maxDeadZonePercent, 0.f, 1.f);
	}


}


//-----------------------------------------------------------------------------------------------
// Getters and Setters
//
float AnalogJoystick::GetCorrectedX() const {
	return m_positionCorrectedNormalized.x;
}

float AnalogJoystick::GetCorrectedY() const {
	return m_positionCorrectedNormalized.y;
}

float AnalogJoystick::GetAngle() const {
	return m_angleDegrees;
}

float AnalogJoystick::GetMagnitude() const {
	return m_magnitude;
}