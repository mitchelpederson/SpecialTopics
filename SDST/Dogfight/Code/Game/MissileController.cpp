#include "Game/MissileController.hpp"
#include "Game/GameCommon.hpp"

//----------------------------------------------------------------------------------------------------------------
MissileController::MissileController() {

}


//----------------------------------------------------------------------------------------------------------------
MissileController::~MissileController() {

}


//----------------------------------------------------------------------------------------------------------------
void MissileController::Update() {
	CheckIfExpired();
	if ( entity->IsAlive() && entity->def.GetFlightStyle() == FLIGHT_MISSILE ) {
		TurnTowardTarget();
	}
}


//----------------------------------------------------------------------------------------------------------------
void MissileController::TurnTowardTarget() {
	throttle = 1.f;
	rollAxis = 0.f;
	yawAxis = 0.f;
	pitchAxis = 0.f;

	Entity* target = TheGame::GetMultiplayerState()->GetEntityByID( entity->currentState.lockedEntityID );
	Vector3 targetPos = target->GetPosition();
	Vector3 targetVel = target->GetVelocity();

	Vector3 displacementToTarget = targetPos - entity->GetPosition();
	Vector3 missileVelocityOnDisplacement = entity->GetVelocity() * DotProduct( displacementToTarget.GetNormalized(), entity->GetVelocity() );

	float timeToImpact = displacementToTarget.GetLength() / missileVelocityOnDisplacement.GetLength();
	Vector3 estimatedTargetPosition = targetPos + (targetVel.GetNormalized() * timeToImpact);

	Vector3 displacementToEstimatedTargetPosition = estimatedTargetPosition - entity->GetPosition();

	Vector3 newForward = displacementToEstimatedTargetPosition.GetNormalized();
	Vector3 newRight = Vector3::CrossProduct( Vector3::UP, newForward ).GetNormalized();
	Vector3 newUp = Vector3::CrossProduct( newForward, newRight ).GetNormalized();
	Matrix44 rotation( newRight, newUp, newForward );

	entity->currentState.transform.TurnToward( entity->currentState.transform.GetLocalToWorldMatrix(), rotation, entity->def.GetMaxRollSpeed() * g_theGame->GetDeltaTime() );
}


//----------------------------------------------------------------------------------------------------------------
void MissileController::CheckIfExpired() {
	if ( entity->currentState.age > entity->def.GetLifespan() ) {
		entity->Kill(-1);
		return;
	}

	if ( !TheGame::GetMultiplayerState()->DoesEntityExist( entity->currentState.lockedEntityID ) ) {
		entity->Kill(-1);
		return;
	}

	Entity* target = TheGame::GetMultiplayerState()->GetEntityByID( entity->currentState.lockedEntityID );
	Vector3 displacement = target->GetPosition() - entity->GetPosition();
	float missileLostTargetCheck = DotProduct( entity->currentState.transform.GetWorldForward(), displacement );
	if ( missileLostTargetCheck < 0.f ) {
		entity->Kill(-1);
		return;
	}
}