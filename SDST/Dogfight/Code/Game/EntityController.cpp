#include "Game/EntityController.hpp"
#include "Game/PlayerController.hpp"
#include "Game/NetController.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/InputSystem/InputSystem.hpp"

//----------------------------------------------------------------------------------------------------------------
EntityController::EntityController() {
	
}


//----------------------------------------------------------------------------------------------------------------
EntityController::~EntityController() {
	if ( entity != nullptr ) {
		entity->controller = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
void EntityController::AssignToEntity( Entity* ent ) {
	entity = ent;
	entity->controller = this;
}


//----------------------------------------------------------------------------------------------------------------
void EntityController::Update() {
	yawAxis = 1.f;
	rollAxis = 0.f;
	pitchAxis = 0.f;
	throttle = 0.25f;

	if ( entity->currentState.age > entity->def.GetLifespan() ) {
		entity->Kill(-1);
	}
}
 