#pragma once 
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/TRansform.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Camera.hpp"
#include <vector>


struct Particle {
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
	float mass = 1.f;
	float timeBorn;
	float timeWillDie;
	float size = 0.5f;

	void Update( float deltaTime ) {
		Vector3 acceleration = force * (1.f / mass);
		velocity = velocity + (acceleration * deltaTime);
		position = position + (velocity * deltaTime);
		force = Vector3::ZERO;
	}

	bool IsDead( float currentTime ) {
		if (currentTime >= timeWillDie) {
			return true;
		}
		else {
			return false;
		}
	}


	float GetNormalizedAge( float currentTime ) {
		return (currentTime - timeBorn) / (timeWillDie - timeBorn);
	}
};


class ParticleEmitter {
public:

	ParticleEmitter();
	ParticleEmitter( Clock* clock );
	void Update();
	void SpawnParticle();
	void SpawnBurst();
	bool IsSafeToDestroy() const;
	void SetBurstAmount( int min, int max );

	void SetSpawnSpeed( FloatRange speed );
	void SetColorOverTime( const Rgba& startColor, const Rgba& endColor );
	void SetLifetime( float min, float max );

	void PreRender( Camera* cam );


public:

	// Rendering stuff
	Transform transform;
	Renderable* renderable = nullptr;
	Mesh* mesh = nullptr;
	std::vector<Particle> particles;
	Clock* clock = nullptr;

	// Emitter Parameters
	float spawnRate = 0.1f;
	IntRange burstSize = IntRange(1, 1);
	float timeAtLastSpawn = -100.f;
	float spawnConeAngle = 30.f;

	// Particle Parameters
	FloatRange particleLifespan = FloatRange(1.f, 10.f);
	Rgba startColor = Rgba(255, 0, 255, 255);
	Rgba endColor = Rgba(0, 255, 0, 255);
	FloatRange spawnSpeed = FloatRange(1.f, 3.f);
	FloatRange particleSize = FloatRange(0.5f, 0.5f);


private:
	
};