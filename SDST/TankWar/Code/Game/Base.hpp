#pragma once
#include "Game/GameObject.hpp"
#include "Game/SwarmEnemy.hpp"

#include <vector>

class Base : public GameObject {

public:
	Base(PlayState* state, eTeam team = TEAM_ENEMY);
	~Base();

	virtual void Update() override;
	virtual void Kill() override;

private:
	void BuildBaseMesh();
	void SpawnChild();

	std::vector<SwarmEnemy*> m_children;

	unsigned int m_maxChildren;
	Stopwatch m_spawnTimer;


};