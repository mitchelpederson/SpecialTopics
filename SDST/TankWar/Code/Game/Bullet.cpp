#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Renderer.hpp"


Bullet::Bullet(PlayState* playState, eTeam team) 
	: GameObject(playState, team)
	, lifeTimer(currentState->m_sceneClock) {
	linearDrag = 0.f;
	angularDrag = 0.f;
	m_collisionRadius = 0.1f;
	BuildBulletMesh();
	lifeTimer.SetTimer(5.f);
}

Bullet::~Bullet() {
	currentState->m_scene->RemoveLight(m_bulletLight);
	delete m_bulletMesh;
	m_bulletMesh = nullptr;
}

void Bullet::BuildBulletMesh() {
	MeshBuilder mb;
	mb.Begin(TRIANGLES, true);
	mb.AddSphere(Vector3::ZERO, m_collisionRadius, 6, 6, Rgba(255, 120, 0, 255));
	mb.End();

	m_renderable = new Renderable();
	m_bulletMesh = new Mesh();
	m_bulletMesh->FromBuilderAsType<Vertex3D_Lit>(&mb);
	m_renderable->SetMesh(m_bulletMesh);
	m_renderable->SetMaterial(g_theRenderer->GetMaterial("player"));
	currentState->m_scene->AddRenderable(m_renderable);

	m_bulletLight = new Light();
	m_bulletLight->SetAsPointLight(transform.position, Rgba(200, 200, 0, 255), 1.f, 0.99f);
	currentState->m_scene->AddLight(m_bulletLight);
}


void Bullet::Update() {
	GameObject::Update();
	m_bulletLight->m_position = transform.position;
	if (lifeTimer.HasElapsed()) {
		Kill();
	}

	if (currentState->testGameMap->GetHeightAboveTerrainAtPoint(transform.position) <= 0.f) {
		Kill();
	}
}


void Bullet::Kill() {
	GameObject::Kill();
}


float Bullet::GetDamage() const {
	return m_damage;
}