#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"

ParticleEmitter::ParticleEmitter() : clock(new Clock(g_masterClock)), renderable(new Renderable()) {}


ParticleEmitter::ParticleEmitter( Clock* clock ) : clock(clock), renderable(new Renderable()) {}


void ParticleEmitter::SpawnParticle() {
	Particle p;
	p.position = Vector3::ZERO;
	p.velocity = GetRandomDirectionInCone(spawnConeAngle) * spawnSpeed.GetRandomInRange();
	p.timeBorn = clock->total.seconds;
	p.timeWillDie = p.timeBorn + particleLifespan.GetRandomInRange();
	p.size = particleSize.GetRandomInRange();
	particles.push_back(p);
}


void ParticleEmitter::SetSpawnSpeed( FloatRange speed ) {
	spawnSpeed = speed;
}


void ParticleEmitter::SetColorOverTime( const Rgba& start, const Rgba& end ) {
	startColor = start;
	endColor = end;
}


void ParticleEmitter::SetLifetime( float min, float max ) {
	particleLifespan = FloatRange(min, max);
}


void ParticleEmitter::SetBurstAmount( int min, int max ) {
	burstSize = IntRange(min, max);
}


void ParticleEmitter::SpawnBurst() {
	int currentBurstSize = burstSize.GetRandomInRange();
	for (int i = 0; i < currentBurstSize; i++) {
		SpawnParticle();
	}
}


bool ParticleEmitter::IsSafeToDestroy() const {
	return (spawnRate == 0) && (particles.size() == 0);
}



void ParticleEmitter::Update() {

	if (clock->total.seconds - timeAtLastSpawn > spawnRate) {
		int timesElapsed = (int) ((clock->total.seconds - timeAtLastSpawn) / spawnRate);
		for (int i = 0; i < timesElapsed; i++) {
			SpawnParticle();
		}
		timeAtLastSpawn = clock->total.seconds;
	}

	for (Particle& p : particles) {
		//p.force = Vector3(0.f, -1.f, 0.f);
		p.Update(clock->frame.seconds);
	}

	for (int index = particles.size() - 1; index >= 0; index--) {
		if (particles[index].IsDead( clock->total.seconds )) {
			particles[index] = particles[particles.size() - 1];
			particles.pop_back();
		}
	}
}


void ParticleEmitter::PreRender( Camera* camera ) {

	if (mesh != nullptr) {
		delete mesh;
	}
	mesh = new Mesh();
	MeshBuilder builder;
	builder.Begin(TRIANGLES, false);
	Matrix44 cameraModel = camera->transform.GetLocalToWorldMatrix();
	Matrix44 particleModel = transform.GetWorldToLocalMatrix();
	particleModel.Append(cameraModel);

	Vector3 up = particleModel.GetUp();
	Vector3 right = particleModel.GetRight();

	for (int particleIndex = 0; particleIndex < particles.size(); particleIndex++) {
		Particle& p = particles[particleIndex];

		Vector3 bl = p.position - (up * p.size) - (right * p.size);
		Vector3 br = p.position - (up * p.size) + (right * p.size);
		Vector3 tr = p.position + (up * p.size) + (right * p.size);
		Vector3 tl = p.position + (up * p.size) - (right * p.size);
		Rgba color = Interpolate( startColor, endColor, p.GetNormalizedAge( clock->total.seconds ) );

		builder.SetColor(color);
		builder.PushQuad(bl, br, tr, tl);
	}

	builder.End();
	mesh->FromBuilderAsType<Vertex3D_PCU>(&builder);
	renderable->SetMesh(mesh);
	renderable->SetModelMatrix(transform.GetLocalToWorldMatrix());
}