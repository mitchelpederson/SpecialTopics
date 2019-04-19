#pragma once
#include "Game/GameState/GameState.hpp"


class SetupState : public GameState {

public:
	SetupState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;

private:

	void UpdateCursorPosition();

	unsigned int m_campaignSelectCursor = 0;
	unsigned int m_numCampaigns = 0;

};
