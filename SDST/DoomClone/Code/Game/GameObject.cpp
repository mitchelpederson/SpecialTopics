#include "Game/GameObject.hpp"
#include "Game/GameCommon.hpp"


GameObject::GameObject() { }


GameObject::~GameObject() {
	delete m_renderable;
	m_renderable = nullptr;
}

void GameObject::SetRenderable( Renderable* renderable ) {
	m_renderable = renderable;
}

void GameObject::SetPosition( const Vector3& position ) {
	transform.position = position;
	m_renderable->SetModelMatrix(transform.GetLocalToWorldMatrix());
}


Renderable* GameObject::GetRenderable() {
	return m_renderable;
}


const Vector3& GameObject::GetPosition() {
	return transform.position;
}


void GameObject::UpdatePhysics() {

	float dt = g_masterClock->frame.seconds;
	Vector3 linearAcceleration = frameLinearForce * (1.f / mass);
	Vector3 angularAcceleration = frameTorque * (1.f / mass);

	// Apply linear force
	linearVelocity = (linearVelocity) + (linearAcceleration * dt);
	linearVelocity = linearVelocity * (1.f - (linearDrag * dt));
	transform.position = transform.position + (linearVelocity * dt);
	frameLinearForce = Vector3();

	// Apply angular force
	angularVelocity = angularVelocity + (angularAcceleration * dt);
	angularVelocity = angularVelocity * (1.f - (angularDrag * dt));
	transform.Rotate(angularVelocity * dt);
	frameTorque = Vector3();
}


void GameObject::AddForce( const Vector3& force ) {
	frameLinearForce = frameLinearForce + force;
}


void GameObject::AddTorque( const Vector3& torque ) {
	frameTorque = frameTorque + torque;
}


bool GameObject::IsDeletable() {
	return m_isDeletable;
}

void GameObject::Kill() {
	m_isDeletable = true;
	g_theGame->m_scene->RemoveRenderable(m_renderable);
}