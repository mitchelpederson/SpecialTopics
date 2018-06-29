#pragma once

#include "Game/GameObject.hpp"

class Base;

class SwarmEnemy : public GameObject {

public:
	SwarmEnemy(PlayState* state, Base* parent, eTeam team = TEAM_ENEMY);
	~SwarmEnemy() override;

	virtual void Update() override;
	virtual void Kill() override;

	void SignalParentDied();
	float GetDamageToPlayer() const;

private:
	void BuildEnemyMesh();
	void SwarmPlayer();

	Base* m_parent;
	float m_damageToPlayer = 5.f;

	float m_moveSpeed = 20.f;

};