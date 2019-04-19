#pragma once
#include "Game/GameState/GameState.hpp"

#include "Engine/UI/TextBox.hpp"

class MenuHostState : public GameState {

public:
	MenuHostState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;


private:
	TextBox m_nameInputBox;
};