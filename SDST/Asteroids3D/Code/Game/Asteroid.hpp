#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class Asteroid : public GameObject {
public:
	Asteroid();
	~Asteroid();

	void Update();
	Asteroid* CreateChild();
	void Hit();
	virtual void Kill() override;
	void SetScale(float scale);

	float GetAsteroidScale();

private:

	SoundID explosionSound;
	SoundID hitSound;

	void SetUpAsteroidRenderable();

	float m_scale = 30.f;
	float m_health = 20.f;
};