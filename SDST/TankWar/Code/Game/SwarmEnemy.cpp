#include "Game/GameCommon.hpp"
#include "Game/SwarmEnemy.hpp"
#include "Engine/Renderer/DebugRender.hpp"

SwarmEnemy::SwarmEnemy(PlayState* state, Base* parent, eTeam team /* = TEAM_ENEMY */)
	: GameObject(state, team)
	, m_parent(parent)
{
	m_collisionRadius = 0.4f;
	m_moveSpeed = SWARMER_MOVE_SPEED;
	BuildEnemyMesh();
}

SwarmEnemy::~SwarmEnemy() {

}


void SwarmEnemy::BuildEnemyMesh() {
	MeshBuilder mb;
	mb.Begin(TRIANGLES, true);
	mb.AddSphere(Vector3(0.f, 0.25f, 0.f), m_collisionRadius, 6, 6, Rgba(255, 100, 0, 255));
	mb.End();

	Mesh* mesh = new Mesh();
	mesh->FromBuilderAsType<Vertex3D_Lit>(&mb);
	m_renderable = new Renderable();
	m_renderable->SetMesh(mesh);
	m_renderable->SetMaterial(g_theRenderer->GetMaterial("player"));
	currentState->m_scene->AddRenderable(m_renderable);
}


void SwarmEnemy::Update() {
	GameObject::Update();

	SwarmPlayer();
	transform.position.y = m_collisionRadius + g_theGame->GetCurrentPlayState()->testGameMap->GetHeightAtPoint(Vector2(transform.position.x, transform.position.z));
	transform.LookToward(transform.GetLocalToWorldMatrix().GetForward());

	if (g_theGame->IsDevModeActive()) {
		DebugRenderLineSegment(1.f, transform.position, Rgba(255,0,0,255), transform.position + (Vector3::UP * 30.f), Rgba(255, 0, 0, 255));
	}
}


void SwarmEnemy::Kill() {
	GameObject::Kill();
}


void SwarmEnemy::SwarmPlayer() {

	// Follow player
	Vector3 playerLocation = currentState->player->transform.position;
	Vector3 direction = playerLocation - transform.position;
	direction.Normalize();
	AddForce((direction * m_moveSpeed) - linearVelocity);

	// Do the flocking stuff
	std::vector<SwarmEnemy*> neighbors = currentState->GetSwarmersInRadius(GetPosition(), SWARMER_FLOCK_RADIUS);

	if (neighbors.size() > 1) {
		Vector3 separationForce = Vector3::ZERO;
		Vector3 averagePosition = Vector3::ZERO;
		Vector3 averageVelocity = Vector3::ZERO;
		for (int neighborIndex = 0; neighborIndex < neighbors.size(); neighborIndex++) {
			SwarmEnemy* other = neighbors[neighborIndex];
			if (this != other) {
				averagePosition += neighbors[neighborIndex]->transform.position; // For cohesiveness
				averageVelocity += neighbors[neighborIndex]->linearVelocity;	// For alignment

				// Get the separation force
				separationForce += (transform.position - neighbors[neighborIndex]->transform.position).GetNormalized();
			}
		}

		if (separationForce.GetLengthSquared() > 0.f) {
			AddForce((separationForce.GetNormalized() * SWARMER_SEPARATION_FACTOR) - linearVelocity);
		}

		averagePosition = averagePosition * ( 1.f / (float) neighbors.size() );
		averageVelocity = averageVelocity * ( 1.f / (float) neighbors.size() );

		AddForce(((averagePosition - transform.position).GetNormalized() * SWARMER_COHESION_FACTOR) - linearVelocity);
		if (averageVelocity.GetLengthSquared() > 0.f) {
			AddForce((averageVelocity.GetNormalized() * SWARMER_ALIGNMENT_FACTOR) - linearVelocity);
		}
	}
}


void SwarmEnemy::SignalParentDied() {
	m_parent = nullptr;
	m_moveSpeed += 20.f;

}


float SwarmEnemy::GetDamageToPlayer() const {
	return m_damageToPlayer;
}