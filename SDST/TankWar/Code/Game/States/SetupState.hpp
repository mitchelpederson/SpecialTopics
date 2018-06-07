#pragma once
#include "Game/States/GameState.hpp"


class SetupState : public GameState {

public:
	SetupState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() const override;

private:

};
