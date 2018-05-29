#pragma once
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderable.h"

class GameObject {

public:
	GameObject();
	~GameObject();

	// Mutators
	void SetRenderable( Renderable* renderable );
	void SetPosition( const Vector3& position );

	void UpdatePhysics();
	void AddForce( const Vector3& force );
	void AddTorque( const Vector3& torque );

	virtual void Kill();

	// Accessors
	Renderable* GetRenderable();
	const Vector3& GetPosition();
	bool IsDeletable();


	// Public members
	Transform transform = Transform();

protected:
	Renderable* m_renderable = nullptr;
	bool m_isDeletable = false;

	// Physics variables
	Vector3 frameLinearForce = Vector3();
	Vector3 frameTorque = Vector3();
	Vector3 linearVelocity = Vector3();
	Vector3 angularVelocity = Vector3();
	float mass = 1.f;
	float linearDrag = 0.5f;
	float angularDrag = 0.9f;

};