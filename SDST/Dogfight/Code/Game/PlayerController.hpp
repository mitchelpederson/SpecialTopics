#pragma once
#include "Game/EntityController.hpp"

struct PlayerInput_T {
	float throttle;
	float yawAxis;
	float pitchAxis;
	float rollAxis;
};

class PlayerController : public EntityController {
public:
	PlayerController();
	~PlayerController();

	virtual void Update() override;

private:

};