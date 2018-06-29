#pragma once
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Audio/AudioCue.hpp"

#include "Game/GameObject.hpp"
#include "Game/States/PlayState.hpp"


class Tank : public GameObject {
public:
	Tank(PlayState* state, eTeam team);
	~Tank();

	virtual void Update() override;
	virtual void Kill() override;

private:

	void CreateTankRenderable();
	void ProcessPlayerInput();

	void UpdateTurret();

	Renderable* m_turretRenderable;
	Transform m_turretTransform;
	Stopwatch m_shootingTimer;

	AudioCue* m_cannonSound = nullptr;
};
