#include "Game/GameCommon.hpp"
#include "Game/Base.hpp"


Base::Base(PlayState* state, eTeam team /* = TEAM_ENEMY */)
	: GameObject(state, team)
	, m_spawnTimer(state->m_sceneClock)
{
	m_collisionRadius = 0.7f;
	BuildBaseMesh();
	m_spawnTimer.SetTimer(BASE_SPAWN_RATE);
	m_maxHealth = BASE_HEALTH;
	m_currentHealth = m_maxHealth;
	m_maxChildren = BASE_MAX_SPAWNS;
}


Base::~Base() {
	
}

void Base::BuildBaseMesh() {
	MeshBuilder mb;
	mb.Begin(TRIANGLES, true);
	mb.AddCube(Vector3(0.f, 0.25f, 0.f), Vector3(0.7f, 0.5f, 0.7f), Rgba(255, 0, 0, 255));
	mb.AddSphere(Vector3(0.f, 0.6f, 0.f), 0.25f, 6, 6, Rgba(255, 100, 0, 255));
	mb.End();

	Mesh* mesh = new Mesh();
	mesh->FromBuilderAsType<Vertex3D_Lit>(&mb);
	m_renderable = new Renderable();
	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(g_theRenderer->GetMaterial("player"));
	currentState->m_scene->AddRenderable(m_renderable);
}


void Base::Update() {
	GameObject::Update();

	transform.position.y = 0.125f + g_theGame->GetCurrentPlayState()->testGameMap->GetHeightAtPoint(Vector2(transform.position.x, transform.position.z));

	if (m_spawnTimer.Decrement() && m_children.size() < m_maxChildren) {
		SpawnChild();
	}
}


void Base::Kill() {
	GameObject::Kill();
	for (unsigned int i = 0; i < m_children.size(); i++) {
		m_children[i]->SignalParentDied();
	}
}


void Base::SpawnChild() {
	SwarmEnemy* child = currentState->SpawnSwarmEnemyAtSpot(transform.position, this);
	m_children.push_back(child);
}
