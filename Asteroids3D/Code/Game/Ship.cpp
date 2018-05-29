#include "Game/Ship.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Light.hpp"

Ship::Ship(FirstPersonCamera* followCamera) : m_followingCamera(followCamera) {
	SetUpShipRenderable();
	followCamera->transform.parent = &transform;
	m_followingCamera->transform.position = Vector3(0.f, 3.f, -14.f);

	shootSound = g_audioSystem->CreateOrGetSound("Data/Audio/asteroids-shoot.wav");
	thrustSound = g_audioSystem->CreateOrGetSound("Data/Audio/asteroids-thrust.wav");
	strafeSound = g_audioSystem->CreateOrGetSound("Data/Audio/asteroids-strafe.wav");
	playingThrustSound = g_audioSystem->PlaySound(thrustSound, true);
	g_audioSystem->SetSoundPlaybackSpeed(playingThrustSound, 0.2f);
	g_audioSystem->SetSoundPlaybackVolume(playingThrustSound, 0.01f);
}


Ship::~Ship() {
	m_followingCamera = nullptr;	
}


void Ship::SetUpShipRenderable() {
	m_renderable = new Renderable();

	m_renderable->SetMaterial( g_theRenderer->GetMaterial("ship") );
	m_renderable->SetMesh( g_theRenderer->CreateOrGetMesh("Data/Models/ship/ship.obj") );
	g_theGame->m_scene->AddRenderable( m_renderable );

	m_leftEmitter  = new ParticleEmitter(g_theGame->m_gameClock);
	m_rightEmitter = new ParticleEmitter(g_theGame->m_gameClock);
	Material* p = new Material(g_theRenderer->GetShader("additive"));
	p->SetTexture(0, g_theRenderer->CreateOrGetTexture("Data/Images/particle.png"));
	m_leftEmitter->renderable->SetMaterial(g_theRenderer->GetMaterial("particle"));
	m_rightEmitter->renderable->SetMaterial(g_theRenderer->GetMaterial("particle"));
	m_leftEmitter->SetColorOverTime(Rgba(200, 200, 20, 255), Rgba(140, 0, 0, 255));
	m_leftEmitter->SetLifetime(0.5f, 1.f);
	m_leftEmitter->spawnConeAngle = 15.f;
	m_leftEmitter->spawnRate = 0.05f;
	m_leftEmitter->spawnSpeed = 4.f;
	m_leftEmitter->particleSize = FloatRange(0.2f, 0.4f);
	m_rightEmitter->SetColorOverTime(Rgba(200, 200, 20, 255), Rgba(140, 0, 0, 255));
	m_rightEmitter->SetLifetime(0.5f, 1.f);
	m_rightEmitter->spawnConeAngle = 15.f;
	m_rightEmitter->spawnRate = 0.05f;
	m_rightEmitter->spawnSpeed = 4.f;
	m_rightEmitter->particleSize = FloatRange(0.2f, 0.4f);
	m_leftEmitter->transform.parent = &transform;
	m_rightEmitter->transform.parent = &transform;
	m_leftEmitter->transform.position  = Vector3(-0.8f, 0.f, -3.5f);
	m_rightEmitter->transform.position = Vector3( 0.8f, 0.f, -3.5f);
	m_leftEmitter->transform.euler  = Vector3(0.f, 90.f, 0.f);
	m_rightEmitter->transform.euler = Vector3(0.f, 90.f, 0.f);

	g_theGame->m_scene->AddParticleEmitter(m_leftEmitter);
	g_theGame->m_scene->AddParticleEmitter(m_rightEmitter);

	m_leftThrustLight = new Light();
	m_rightThrustLight = new Light();
	
	g_theGame->m_scene->AddLight(m_leftThrustLight);
	g_theGame->m_scene->AddLight(m_rightThrustLight);
	m_leftThrustLight->SetAsPointLight(Vector3(-0.8f, 0.f, -3.5f), Rgba(200, 200, 0, 255));
	m_rightThrustLight->SetAsPointLight(Vector3(0.8f, 0.f, -3.5f), Rgba(200, 200, 0, 255));
	leftLightTransform.parent = &transform;
	rightLightTransform.parent = &transform;
	leftLightTransform.position = Vector3(-0.8f, 0.f, -3.5f);
	rightLightTransform.position = Vector3(0.8f, 0.f, -3.5f);

}


void Ship::Update() {

	UpdatePhysics();

	Matrix44 shipTransform = transform.GetLocalToWorldMatrix();
	m_renderable->SetModelMatrix( transform.GetLocalToWorldMatrix() );

	float thrustPercent = g_theInputSystem->GetController(0).GetRightTrigger();
	float emitterSpawnRate = RangeMapFloat(thrustPercent, 0.f, 1.f, 0.3f, 0.01f);
	float emitterSpawnSpeed = RangeMapFloat(thrustPercent, 0.f, 1.f, 1.f, 10.f);

	g_audioSystem->SetSoundPlaybackSpeed(playingThrustSound, RangeMapFloat(thrustPercent, 0.f, 1.f, 0.2f, 1.f));
	g_audioSystem->SetSoundPlaybackVolume(playingThrustSound, RangeMapFloat(thrustPercent, 0.f, 1.f, 0.8f, 1.f));
	m_leftEmitter->spawnRate = emitterSpawnRate;
	m_leftEmitter->spawnSpeed = emitterSpawnSpeed;
	m_rightEmitter->spawnRate = emitterSpawnRate;
	m_rightEmitter->spawnSpeed = emitterSpawnSpeed;

	m_leftEmitter->Update();
	m_rightEmitter->Update();

	m_leftThrustLight->SetAsPointLight(leftLightTransform.GetLocalToWorldMatrix().GetTranslation(), Rgba(200, 200, 0, 255), thrustPercent);
	m_rightThrustLight->SetAsPointLight(rightLightTransform.GetLocalToWorldMatrix().GetTranslation(), Rgba(200, 200, 0, 255), thrustPercent);

}


void Ship::StrafeDashLeft() {
	if (g_theGame->GetElapsedTime() - m_timeAtLastStrafe > m_strafeDelay) {
		AddForce(transform.GetLocalToWorldMatrix().GetRight() * -12000.f);
		m_timeAtLastStrafe = g_theGame->GetElapsedTime();
		g_audioSystem->PlaySound(strafeSound);
	}
}

void Ship::StrafeDashRight() {
	if (g_theGame->GetElapsedTime() - m_timeAtLastStrafe > m_strafeDelay) {
		AddForce(transform.GetLocalToWorldMatrix().GetRight() * 12000.f);
		m_timeAtLastStrafe = g_theGame->GetElapsedTime();
		g_audioSystem->PlaySound(strafeSound);
	}
}


void Ship::Shoot() {
	if (g_theGame->GetElapsedTime() - m_timeAtLastShot > m_firingDelay) {

		g_audioSystem->PlaySound(shootSound);
		Vector3 shipPos = transform.position;
		Vector3 shipForward = transform.GetLocalToWorldMatrix().GetForward();
		Vector3 shipRight = transform.GetLocalToWorldMatrix().GetRight();

		Vector3 bulletSpawnPos = shipPos + (shipForward * 5.f);
		if (m_shotIsLeft) {
			bulletSpawnPos = bulletSpawnPos - (shipRight * 2.f);
		}
		else {
			bulletSpawnPos = bulletSpawnPos - (shipRight * -2.f);
		}
		m_shotIsLeft = !m_shotIsLeft;

		Vector3 bulletLinearForce = (shipForward * 40000.f) + linearVelocity;
		Vector3 bulletTorque = shipForward * 100.f;

		Bullet* bullet = new Bullet();
		bullet->SetPosition(bulletSpawnPos);
		bullet->transform.euler = transform.euler;
		bullet->AddForce(bulletLinearForce);
		bullet->AddTorque(bulletTorque);
		g_theGame->AddBullet(bullet);
		m_timeAtLastShot = g_theGame->GetElapsedTime();
	}
}