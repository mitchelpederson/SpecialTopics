#include "Game/AIController.hpp"
#include "Game/GameCommon.hpp"

//----------------------------------------------------------------------------------------------------------------
AIController::AIController() {

}


//----------------------------------------------------------------------------------------------------------------
AIController::~AIController() {

}


//----------------------------------------------------------------------------------------------------------------
void AIController::Update() {
	Entity* target = TheGame::GetMultiplayerState()->GetEntityByID( entity->currentState.lockedEntityID );
	if ( target != nullptr ) {
		DoHomingBehavior();
	}

	else {
		throttle = 0.5f;
	}
}


//----------------------------------------------------------------------------------------------------------------
void AIController::DoHomingBehavior() {
	throttle = 1.f;
	rollAxis = 0.f;
	yawAxis = 0.f;
	pitchAxis = 0.f;

	Entity* target = TheGame::GetMultiplayerState()->GetEntityByID( entity->currentState.lockedEntityID );

	Vector3 targetForward = target->currentState.transform.GetWorldForward();
	Vector3 myForward = entity->currentState.transform.GetWorldForward();
	Vector3 myUp = entity->currentState.transform.GetWorldUp();
	Vector3 myRight = entity->currentState.transform.GetWorldRight();
	Vector3 directionToTarget = target->currentState.transform.position - entity->currentState.transform.position;
	directionToTarget.Normalize();

	float dotRightDisplacement = DotProduct( myRight, directionToTarget );
	float dotUpDisplacement = DotProduct( myUp, directionToTarget );
	float dotForwardDisplacement = DotProduct( myForward, directionToTarget );

	if ( fabsf( dotRightDisplacement ) >= 0.01f ) {
		yawAxis = -1.f;
	} else if ( dotRightDisplacement <= -0.01f ) {
		yawAxis = 1.f;
	}

	if ( dotUpDisplacement >= 0.01f ) {
		pitchAxis = -1.f;
	} else if ( dotUpDisplacement <= -0.01f ) {
		pitchAxis = 1.f;
	}

	throttle = RangeMapFloat( dotForwardDisplacement, -1.f, 1.f, 0.f, 1.f );

}