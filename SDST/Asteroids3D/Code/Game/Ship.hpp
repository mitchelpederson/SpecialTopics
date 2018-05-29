#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Renderer/Light.hpp"


class Ship : public GameObject {

public:
	Ship(FirstPersonCamera* followCamera);
	~Ship();
	
	void Update();

	void StrafeDashLeft();
	void StrafeDashRight();
	void Shoot();
private:

	SoundID shootSound;
	SoundID thrustSound;
	SoundID strafeSound;
	SoundPlaybackID playingThrustSound;

	FirstPersonCamera* m_followingCamera = nullptr;
	void SetUpShipRenderable();
	
	ParticleEmitter* m_leftEmitter = nullptr;
	ParticleEmitter* m_rightEmitter = nullptr;

	Light* m_leftThrustLight = nullptr;
	Light* m_rightThrustLight = nullptr;
	Transform rightLightTransform;
	Transform leftLightTransform;

	const float m_maxLinearSpeed = 20.f;
	const float m_masAngularSpeed = 20.f;

	bool m_shotIsLeft = true;
	float m_timeAtLastShot = -20.f;
	float m_firingDelay = 0.1f;
	float m_timeAtLastStrafe = -20.f;
	float m_strafeDelay = 2.f;
};