#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Renderer/Light.hpp"


class Bullet : public GameObject {
public:
	Bullet();
	~Bullet();

	void Update();
	virtual void Kill() override;


private:

	void SetUpBulletRenderable();

	float m_lifespan = 5.f;
	Light* m_light = nullptr;
};