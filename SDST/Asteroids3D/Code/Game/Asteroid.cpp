#include "Game/Asteroid.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
Asteroid::Asteroid() {
	SetUpAsteroidRenderable();
	m_scale = GetRandomFloatInRange( 20.f, 40.f );
	transform.Scale(m_scale);
	linearDrag = 0.07f;
	angularDrag = 0.f;
	explosionSound = g_audioSystem->CreateOrGetSound("Data/Audio/asteroids-explosion.wav");
	hitSound = g_audioSystem->CreateOrGetSound("Data/Audio/asteroids-hit.wav");
}


Asteroid::~Asteroid() {

}


void Asteroid::Kill() {
	GameObject::Kill();
	g_theGame->RemoveAsteroid(this);
	g_audioSystem->PlaySound(explosionSound);

	if (m_scale > 10.f) {
		float halfScale = m_scale * 0.5f;
		int numToSpawn = GetRandomIntInRange(2, 5);

		for (int child = 0; child < numToSpawn; child++) {
			Asteroid* childAsteroid = new Asteroid();
			childAsteroid->SetScale(halfScale + GetRandomFloatInRange(-2.f, 2.f));

			Vector3 randomDirection = GetRandomUnitVector();
			Vector3 spawnPos = transform.position + (randomDirection * 5.f);
			Vector3 spawnForce = (randomDirection * 3000.f) + linearVelocity;

			childAsteroid->SetPosition(spawnPos);
			childAsteroid->AddForce(spawnForce);
			childAsteroid->AddTorque(spawnForce);

			g_theGame->AddAsteroid(childAsteroid);
			g_theGame->m_scene->AddRenderable(childAsteroid->m_renderable);
		}

	}
}

void Asteroid::Update() {
	UpdatePhysics();

	m_renderable->SetModelMatrix(transform.GetLocalToWorldMatrix());
}


void Asteroid::SetUpAsteroidRenderable() {
	Mesh* mesh = new Mesh();
	MeshBuilder mb;
	mb.BuildDeformedSphere(mesh, Vector3::ZERO, 1.f, 10, 10, 0.6f);
	m_renderable = new Renderable();
	m_renderable->SetMaterial( g_theRenderer->GetMaterial("asteroid") );
	m_renderable->SetMesh(mesh);
}


void Asteroid::Hit() {
	float distanceToPlayer = (g_theGame->GetPlayerLocation() - transform.position).GetLength();
	g_audioSystem->PlaySound(hitSound, false, RangeMapFloat(distanceToPlayer, 0.f, 1200.f, 0.5f, 0.f));
	m_health -= 1.f;
	if (m_health <= 0.f) {
		Kill();
	}
}


float Asteroid::GetAsteroidScale() {
	return m_scale;
}


void Asteroid::SetScale(float scale) {
	m_scale = scale;
	transform.scale = Vector3(m_scale, m_scale, m_scale);
}