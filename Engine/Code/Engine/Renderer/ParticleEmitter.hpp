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
#include <deque>


class ParticleEmitter;
struct Particle;

typedef void (*particle_update_cb)( Particle* particle, float deltaTime );
typedef void (*emitter_pre_render_cb)( ParticleEmitter* pe, Camera* camera );
typedef void (*emitter_update_cb)( ParticleEmitter* pe );



struct Particle {
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
	float mass = 1.f;
	float timeBorn;
	float timeWillDie;
	float size = 0.5f;
	particle_update_cb Update = Particle::DefaultUpdate;


	static void DefaultUpdate( Particle* particle, float deltaTime ) {
		Vector3 acceleration = particle->force * (1.f / particle->mass);
		particle->velocity = particle->velocity + (acceleration * deltaTime);
		particle->position = particle->position + (particle->velocity * deltaTime);
		particle->force = Vector3::ZERO;
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
	~ParticleEmitter();

	static void DefaultUpdate( ParticleEmitter* pe );
	void SpawnParticle();
	void SpawnBurst();
	bool IsSafeToDestroy() const;
	void SetBurstAmount( int min, int max );

	void SetSpawnSpeed( FloatRange speed );
	void SetColorOverTime( const Rgba& startColor, const Rgba& endColor );
	void SetLifetime( float min, float max );

	static void DefaultPreRender( ParticleEmitter* pe, Camera* cam );


public:
	emitter_update_cb Update = ParticleEmitter::DefaultUpdate;
	emitter_pre_render_cb PreRender = ParticleEmitter::DefaultPreRender;
	particle_update_cb UpdateParticle = Particle::DefaultUpdate;

	// Rendering stuff
	Transform transform;
	Renderable* renderable = nullptr;
	Mesh* mesh = nullptr;
	std::deque<Particle> particles;
	Clock* clock = nullptr;

	// Emitter Parameters
	float spawnRate = 0.1f;
	IntRange burstSize = IntRange(1, 1);
	float timeAtLastSpawn = -100.f;
	float spawnConeAngle = 30.f;
	bool spawnInWorldSpace = false;

	// Particle Parameters
	FloatRange particleLifespan = FloatRange(1.f, 10.f);
	Rgba startColor = Rgba(255, 0, 255, 255);
	Rgba endColor = Rgba(0, 255, 0, 255);
	FloatRange spawnSpeed = FloatRange(1.f, 3.f);
	FloatRange particleSize = FloatRange(0.5f, 0.5f);


private:
	
};


