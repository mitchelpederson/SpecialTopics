#pragma once
#include "Game/EntityController.hpp"


class AIController : public EntityController {

public:
	AIController();
	~AIController();

	virtual void Update() override;

	void DoHomingBehavior();
};