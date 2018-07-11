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

	void UpdateCursorPosition();
	void SelectCampaign();

	unsigned int m_campaignSelectCursor = 0;
	unsigned int m_numCampaigns = 0;

	std::vector<std::string> m_campaignNames;
};
