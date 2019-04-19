#pragma once
#include "Game/Entity.hpp"

#include "Engine/InputSystem/XboxController.hpp"


class EntityController {
public:
	EntityController();
	~EntityController();

	virtual void Update();
	void AssignToEntity( Entity* ent );

	
public:
	Entity* entity = nullptr;
	int connectionID = -1;

	float		throttle = 0.3f;
	float		rollAxis = 0.f;
	float		pitchAxis = 0.f;
	float		yawAxis = 0.f;

	bool		isFireMissilePressed = false;
	bool		isFireGunPressed = false;

	float		cameraYawAxis = 0.f;
	float		cameraPitchAxis = 0.f;
};
