#include "Game/GameObject.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/DebugRender.hpp"


GameObject::GameObject() : m_breadcrumbInterval( g_theGame->GetCurrentPlayState()->m_sceneClock ) {
	m_breadcrumbInterval.SetTimer(1.f);
}


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

	Vector2 mapDimensions(g_theGame->GetCurrentPlayState()->testGameMap->GetDimensions().x, g_theGame->GetCurrentPlayState()->testGameMap->GetDimensions().y);
	mapDimensions *= g_theGame->GetCurrentPlayState()->testGameMap->GetChunkSize() - 1;
	mapDimensions.x -= 1.f;
	mapDimensions.y -= 1.f;
	if (transform.position.x > mapDimensions.x) {
		transform.position.x = mapDimensions.x;
	}
	else if (transform.position.x < 0.f) {
		transform.position.x = 0.f;
	}
	if (transform.position.z > mapDimensions.y) {
		transform.position.z = mapDimensions.y;
	}
	else if (transform.position.z < 0.f) {
		transform.position.z = 0.f;
	}

	transform.position.y = 1.f + g_theGame->GetCurrentPlayState()->testGameMap->GetHeightAtPoint(Vector2(transform.position.x, transform.position.z));

	if (m_breadcrumbInterval.CheckAndReset()) {
		m_breadcrumbs.push_back(transform.position);
		DebugRenderPoint(3.f, transform.position);
	}
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
	g_theGame->GetCurrentPlayState()->m_scene->RemoveRenderable(m_renderable);
}