#pragma once
#include "Game/EntityController.hpp"


class MissileController : public EntityController {

public:
	MissileController();
	~MissileController();

	virtual void Update() override;

private:
	void TurnTowardTarget();
	void CheckIfExpired();

};