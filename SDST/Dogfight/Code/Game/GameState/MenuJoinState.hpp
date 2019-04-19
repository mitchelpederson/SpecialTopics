#pragma once
#include "Game/GameState/GameState.hpp"

#include "Engine/UI/TextBox.hpp"


class MenuJoinState : public GameState {

public:
	MenuJoinState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;


private:

	TextBox m_ipInputBox;
	TextBox m_nameInputBox;
};