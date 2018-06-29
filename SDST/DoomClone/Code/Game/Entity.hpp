#pragma once
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Sprites/IsoSpriteAnim.hpp"
#include "Game/EntityDefinition.hpp"

class Entity {

public:
	Entity();
	Entity( unsigned char id );

	virtual void	Update();
	virtual void	UpdateSpriteAnim();
	virtual void	Render() const;
	virtual void	RenderMinimap() const;
	virtual void	Kill();
	virtual void	Revive();

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
	bool		IsSolid() const;

	void		Damage(float damageAmount);
	void		Heal(float healAmount);

	void		SetPosition(const Vector2& position);
	void		SetVelocity(const Vector2& velocity);
	void		SetOrientationDegrees(float orientation);
	void		SetCosmeticRadius(float newRadius);
	void		SetPhysicalRadius(float newRadius);

	// Behaviors
	void Shoot();


public:
	EntityDefinition* m_def = nullptr;
	IsoSpriteAnim*	m_currentSpriteAnim = nullptr;
	std::string	m_animSetName;
	Vector2		m_position;
	Vector2		m_velocity;

	float		m_cosmeticRadius;
	float		m_physicalRadius;

	float		m_orientationDegrees = 0.f;
	float		m_spinSpeed = 0.f;

	float		m_age = 0.f;
	float		m_health = 1.f;
	float		m_maxHealth = 1.f;
	unsigned int m_maxAmmo = 999;
	unsigned int m_currentAmmo = 99;
	bool		m_isAlive = true;
	bool		m_isDeleteable = false;
	float		m_ageAtDeath;

	bool		m_isSolid = true;

	Stopwatch	m_firingDelayTimer;
	Stopwatch	m_cantMoveTimer;
	
};