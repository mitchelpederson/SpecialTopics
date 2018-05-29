#pragma once

#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Disc2.hpp"

class Entity {
public:
	Entity();
	Entity(const Vector2& position, const Vector2& velocity, float cosmeticRadius,
		float physicalRadius, float orientationDegrees, float spinSpeed);

	Vector2		GetPosition() const;
	Vector2		GetVelocity() const;
	Disc2		GetCosmeticDisc2() const;
	Disc2		GetPhysicalDisc2() const;

	float		GetOrientationDegrees() const;
	float		GetSpinSpeed() const;
	float		GetAge() const;	

	void		SetPosition(const Vector2& position);

	void		Render() const;
	void		Update(float deltaSeconds);

protected:

	// Entity* childrenEntities;
	Vector2		m_position;
	Vector2		m_velocity;
	
	float		m_cosmeticRadius;
	float		m_physicalRadius;

	float		m_orientationDegrees;
	float		m_spinSpeed;

	float		m_age;
};