#pragma once
#include "Game/States/GameState.hpp"


class MenuState : public GameState {

public:
	MenuState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() const override;

private:

};
