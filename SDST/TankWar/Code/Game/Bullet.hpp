#pragma once

#include "Game/GameObject.hpp"

#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/Light.hpp"

class Bullet : public GameObject {

public:
	Bullet(PlayState* state, eTeam team);
	~Bullet();


	virtual void Update() override;
	virtual void Kill() override;

	float GetDamage() const;

private:
	void BuildBulletMesh();

	Light* m_bulletLight = nullptr;
	Mesh* m_bulletMesh = nullptr;
	Stopwatch lifeTimer;

	float m_damage = 1.f;
};