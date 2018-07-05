#include "Game/Tank.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"


Tank::Tank(PlayState* state, eTeam team) 
	: GameObject(state, team)
	, m_shootingTimer(currentState->m_sceneClock)
{
	CreateTankRenderable();
	m_turretTransform.parent = &transform;
	m_turretTransform.position.y = 0.125f;
	m_collisionRadius = 0.4f;
	m_maxHealth = 500.f;
	m_currentHealth = 500.f;
	m_shootingTimer.SetTimer(0.15f);
	m_cannonSound = new AudioCue(AudioCueDefinition::s_definitions["tank-shot"]);
}


void Tank::CreateTankRenderable() {

	// tank base
	MeshBuilder mb;
	Mesh* tankBase = new Mesh();
	mb.BuildCube(tankBase, Vector3::ZERO, Vector3(0.75f, 0.25f, 1.f), Rgba(0, 120, 255, 255));
	Mesh* turretMesh = new Mesh();

	m_renderable = new Renderable();
	m_renderable->SetMesh(tankBase);
	m_renderable->SetMaterial(g_theRenderer->GetMaterial("player"));
	currentState->m_scene->AddRenderable(m_renderable);

	// turret 
	MeshBuilder turretBuilder;
	Mesh* turret = new Mesh();
	turretBuilder.Begin(TRIANGLES, true);
	turretBuilder.AddCube(Vector3(0.f, 0.f, 0.3f), Vector3(0.1f, 0.1f, 0.5f), Rgba(255, 0, 0, 255));
	turretBuilder.AddSphere(Vector3::ZERO, 0.125f, 8, 8, Rgba(255, 0, 0, 255));
	turretBuilder.End();
	turret->FromBuilderAsType<Vertex3D_Lit>( &turretBuilder );

	m_turretRenderable = new Renderable();
	m_turretRenderable->SetMesh(turret);
	m_turretRenderable->SetMaterial(g_theRenderer->GetMaterial("player"));
	currentState->m_scene->AddRenderable(m_turretRenderable);
}


Tank::~Tank() {
	currentState->m_scene->RemoveRenderable(m_turretRenderable);
	delete m_turretRenderable;
	m_turretRenderable = nullptr;
}


void Tank::ProcessPlayerInput() {
	PROFILER_SCOPED_PUSH();
	Vector3 playerForward = transform.GetLocalToWorldMatrix().GetForward();
	Vector3 playerRight = transform.GetLocalToWorldMatrix().GetRight();
	Vector3 forwardMoveForce = Vector3(PLAYER_MOVE_FORCE, 0.f, 0.f);

	if (g_theInputSystem->IsKeyPressed('W')) {
		AddForce(Vector3(DotProduct(forwardMoveForce, playerForward), 0.f, DotProduct(forwardMoveForce, playerRight)));
	}
	if (g_theInputSystem->IsKeyPressed('S')) {
		AddForce(Vector3(DotProduct(forwardMoveForce * -1.f, playerForward), 0.f, DotProduct(forwardMoveForce * -1.f, playerRight)));
	}

	if (g_theInputSystem->IsKeyPressed('A')) {
		transform.euler.y -= PLAYER_TANK_TURN_RATE * currentState->GetDeltaTime();
	}
	if (g_theInputSystem->IsKeyPressed('D')) {
		transform.euler.y += PLAYER_TANK_TURN_RATE * currentState->GetDeltaTime();
	}

	if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_SPACE)) {
		if (m_shootingTimer.CheckAndReset()) {
			Vector3 turretForward = m_turretTransform.GetWorldForward();
			Vector3 turretPosition = m_turretTransform.GetWorldLocation();
			currentState->SpawnBullet( turretPosition + (turretForward * 0.7f), turretForward, 30.f, TEAM_PLAYER );
			m_cannonSound->Play();
		}
	}
}


void Tank::Update() {
	PROFILER_SCOPED_PUSH();
	if (!DevConsole::GetInstance()->IsOpen() && currentState->currentSubstate == STATE_PLAYING) {
		ProcessPlayerInput();
	}
	

	GameObject::Update();
	transform.position.y = 0.125f + g_theGame->GetCurrentPlayState()->testGameMap->GetHeightAtPoint(Vector2(transform.position.x, transform.position.z));
	UpdateTurret();
	m_turretRenderable->SetModelMatrix(m_turretTransform.GetLocalToWorldMatrix());
}


void Tank::UpdateTurret() {
	PROFILER_SCOPED_PUSH();
	Camera* camera = currentState->m_camera;
	Ray3 ray(camera->transform.position + (camera->m_cameraMatrix.GetForward() * 3.5f), camera->m_cameraMatrix.GetForward());

	// Do raycast
	RaycastHit3 result = currentState->Raycast(1, ray);

	Vector3 turretLocalLookAtTarget = transform.GetWorldToLocalMatrix().TransformPosition(result.position);
	Matrix44 currentTurretMatrix = m_turretTransform.GetLocalToParentMatrix();
	Matrix44 targetLookAt = m_turretTransform.LookAtLocal(turretLocalLookAtTarget, transform.GetLocalToWorldMatrix().GetUp()); 
	
	m_turretTransform.TurnToward(currentTurretMatrix, targetLookAt, PLAYER_TURRET_TURN_RATE * currentState->m_sceneClock->frame.seconds);

	// Do another raycast, this time from the turret's position along its forward
	Ray3 rayTurret(  m_turretTransform.GetWorldLocation() + (m_turretTransform.GetWorldForward() * 0.7f), m_turretTransform.GetWorldForward());
	RaycastHit3 resultTurret = currentState->Raycast(1, rayTurret);
	DebugRenderPoint(0.f, resultTurret.position);
	DebugRenderLineSegment(0.0016f, m_turretTransform.GetWorldLocation(), Rgba(), resultTurret.position, Rgba());

}


void Tank::Kill() {
	if (currentState->currentSubstate == STATE_PLAYING) {
		currentState->SignalPlayerDied();
	}
}