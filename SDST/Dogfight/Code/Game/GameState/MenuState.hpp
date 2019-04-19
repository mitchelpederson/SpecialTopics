#pragma once
#include "Game/GameState/GameState.hpp"


class MenuState : public GameState {

public:
	MenuState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;

private:

	int m_cursor = 0;
	int m_numOfOptions = 2;

};
