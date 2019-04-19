#pragma once
#include "Game/EntityController.hpp"


class NetController : public EntityController {
public:
	NetController();
	~NetController();

	virtual void Update() override;

private:

};