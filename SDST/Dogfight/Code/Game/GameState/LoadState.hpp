#pragma once
#include "Game/GameState/GameState.hpp"


class LoadState : public GameState {

public:
	LoadState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;

private:

	void LoadXMLDefinitions();
	void LoadResources();
	void LoadAudio();

	bool m_isFirstFrame = true;
	bool m_isSecondFrame = false;
};
