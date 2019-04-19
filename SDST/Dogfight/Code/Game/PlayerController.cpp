#include "Game/PlayerController.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/InputSystem/InputSystem.hpp"


//----------------------------------------------------------------------------------------------------------------
PlayerController::PlayerController() {

}


//----------------------------------------------------------------------------------------------------------------
PlayerController::~PlayerController() {

}


//----------------------------------------------------------------------------------------------------------------
void PlayerController::Update() {
	InputSystem* in = g_theInputSystem;

	/*if ( in->WasKeyJustPressed( InputSystem::KEYBOARD_SPACE ) ) {
		entity->FireMissile();
	}

	if ( in->IsKeyPressed( 'Q' ) ) {
		entity->FireMachineGun();
	}*/

	if ( in->WasKeyJustPressed( 'E' ) ) {
		entity->SwitchLockedTarget();
	}

	// Throttle
	if ( in->IsKeyPressed( 'I' ) ) {
		if ( in->IsKeyPressed( InputSystem::KEYBOARD_SHIFT ) ) {	
			throttle = 1.f;
		}
		else {
			throttle = 0.75f;
		}
	}
	else if ( in->IsKeyPressed( 'K' ) ) {

		if ( in->IsKeyPressed( InputSystem::KEYBOARD_SHIFT ) ) {
			throttle = 0.f;
		}
		else {
			throttle = 0.1f;
		}
	}
	else {
		throttle = 0.25f;
	}

	// Roll
	if ( in->IsKeyPressed( 'A' ) ) {
		rollAxis = 1.f;
	}
	else if ( in->IsKeyPressed( 'D' ) ) {
		rollAxis = -1.f;
	}
	else {
		rollAxis = 0.f;
	}


	// Yaw
	if ( in->IsKeyPressed( 'J' ) ) {
		yawAxis = -1.f;
	}
	else if ( in->IsKeyPressed( 'L' ) ) {
		yawAxis =  1.f;
	}
	else {
		yawAxis = 0.f;
	}

	// Pitch
	if ( in->IsKeyPressed( 'W' ) ) {
		pitchAxis = -1.f;
	}
	else if ( in->IsKeyPressed( 'S' ) ) {
		pitchAxis =  1.f;
	}
	else {
		pitchAxis = 0.f;
	}

	isFireMissilePressed = g_theInputSystem->IsKeyPressed( InputSystem::KEYBOARD_SPACE );
	isFireGunPressed = g_theInputSystem->IsKeyPressed( 'Q' );

	const XboxController& xbc = g_theInputSystem->GetController( 0 );
	if ( xbc.IsConnected() ) {
		pitchAxis = -xbc.GetLeftStickY();
		rollAxis = -xbc.GetLeftStickX();
		yawAxis = 0.f;
		if ( xbc.IsButtonPressed( InputSystem::XBOX_LB ) ) {
			yawAxis -= 1.f;
		}
		if ( xbc.IsButtonPressed( InputSystem::XBOX_RB ) ) {
			yawAxis += 1.f;
		}

		throttle = 0.25;
		throttle += 0.75f * xbc.GetRightTrigger();
		throttle -= 0.25f * xbc.GetLeftTrigger();

		cameraPitchAxis = xbc.GetRightStickY();
		cameraYawAxis = xbc.GetRightStickX();

		if ( xbc.IsButtonPressed( InputSystem::XBOX_A ) ) {
			isFireGunPressed = true;
		} else {
			isFireGunPressed = false;
		}

		if ( xbc.WasButtonJustPressed( InputSystem::XBOX_B ) ) {
			isFireMissilePressed = true;
		} else {
			isFireMissilePressed = false;
		}

		if ( xbc.WasButtonJustPressed( InputSystem::XBOX_Y ) ) {
			entity->SwitchLockedTarget();
		}
	}
}
