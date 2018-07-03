#pragma once
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/Renderable.h"


class PlayState;

enum eTeam {
	TEAM_NEUTRAL,
	TEAM_PLAYER,
	TEAM_ENEMY,
	NUM_OF_TEAMS
};

class GameObject {

public:
	GameObject(PlayState* playState, eTeam team = TEAM_NEUTRAL);
	virtual ~GameObject();

	// Mutators
	void SetRenderable( Renderable* renderable );
	void SetPosition( const Vector3& position );

	void UpdatePhysics();
	void AddForce( const Vector3& force );
	void AddTorque( const Vector3& torque );
	void SetForwardVelocity( float speed );

	virtual void Update();
	virtual void Kill();
	virtual void Damage( float amount );

	// Accessors
	Renderable* GetRenderable();
	const Vector3& GetPosition();
	bool IsDeletable();
	float GetCollisionRadius() const;
	float GetNormalizedHealth() const;


	// Public members
	Transform transform = Transform();
	PlayState* currentState = nullptr;

protected:
	Renderable* m_renderable = nullptr;
	bool m_isDeletable = false;

	// Gameplay variables
	float m_maxHealth = 1.f;
	float m_currentHealth = 1.f;

	// Physics variables
	Vector3 frameLinearForce = Vector3();
	Vector3 frameTorque = Vector3();
	Vector3 linearVelocity = Vector3();
	Vector3 angularVelocity = Vector3();
	float mass = 1.f;
	float linearDrag = 4.f;
	float angularDrag = 1.f;
	float m_collisionRadius = 0.4f;


	std::vector<Vector3> m_breadcrumbs;
	Stopwatch m_breadcrumbInterval;
	eTeam m_team = TEAM_NEUTRAL;


};