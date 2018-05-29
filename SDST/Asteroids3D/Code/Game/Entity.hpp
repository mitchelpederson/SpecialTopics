#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"

class Entity {

public:
	Entity();

	void Update(float deltaSeconds);
	void Render() const;

	Vector2		GetPosition() const;
	Vector2		GetVelocity() const;
	Disc2		GetCosmeticDisc2() const;
	Disc2		GetPhysicalDisc2() const;
	AABB2		GetPhysicalAABB2() const;

	float		GetOrientationDegrees() const;
	float		GetSpinSpeed() const;
	float		GetAge() const;	
	float		GetHealth() const;
	bool		IsAlive() const;

	bool		CanFly() const;
	bool		CanSwim() const;
	bool		CanWalk() const;

	//virtual void		Kill() = 0;
	virtual void		Revive();

	void		Damage(float damageAmount);
	void		Heal(float healAmount);

	void		SetPosition(const Vector2& position);
	void		SetVelocity(const Vector2& velocity);
	void		SetOrientationDegrees(float orientation);
	void		SetCosmeticRadius(float newRadius);
	void		SetPhysicalRadius(float newRadius);


private:
	Vector2		m_position;
	Vector2		m_velocity;

	float		m_cosmeticRadius;
	float		m_physicalRadius;

	float		m_orientationDegrees;
	float		m_spinSpeed;

	float		m_age;
	float		m_health;
	float		m_maxHealth;
	bool		m_isAlive;
	float		m_ageAtDeath;

	bool		m_canFly;
	bool		m_canWalk;
	bool		m_canSwim;
};