#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/Entity.hpp"


class Player : public Entity {

public:
	Player();
	void ProcessInput();
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Kill() override;


private:

	void Shoot();

	SpriteSheet* m_gunSpriteSheet = nullptr;


};