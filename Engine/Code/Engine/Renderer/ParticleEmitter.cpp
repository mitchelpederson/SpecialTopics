#include "Engine/Renderer/ParticleEmitter.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"


//----------------------------------------------------------------------------------------------------------------
ParticleEmitter::ParticleEmitter() 
	: clock(new Clock(g_masterClock))
	, renderable(new Renderable()) 
	, timeAtLastSpawn( clock->total.seconds )
{}


//----------------------------------------------------------------------------------------------------------------
ParticleEmitter::ParticleEmitter( Clock* clock ) 
	: clock(clock)
	, renderable(new Renderable()) 
	, timeAtLastSpawn( clock->total.seconds )
{}


//----------------------------------------------------------------------------------------------------------------
ParticleEmitter::~ParticleEmitter() {
	if ( renderable != nullptr ) {
		delete renderable;
		renderable = nullptr;
	}

	if ( mesh != nullptr ) {
		delete mesh;
		mesh = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
void ParticleEmitter::SpawnParticle() {
	Particle p;
	if ( spawnInWorldSpace ) {
		p.position = transform.GetLocalToWorldMatrix().GetTranslation();
	} else {
		p.position = Vector3::ZERO;
	}
	p.velocity = GetRandomDirectionInCone(spawnConeAngle) * spawnSpeed.GetRandomInRange();
	p.timeBorn = clock->total.seconds;
	p.timeWillDie = p.timeBorn + particleLifespan.GetRandomInRange();
	p.size = particleSize.GetRandomInRange();
	p.Update = UpdateParticle;
	particles.push_back(p);
}


//----------------------------------------------------------------------------------------------------------------
void ParticleEmitter::SetSpawnSpeed( FloatRange speed ) {
	spawnSpeed = speed;
}


//----------------------------------------------------------------------------------------------------------------
void ParticleEmitter::SetColorOverTime( const Rgba& start, const Rgba& end ) {
	startColor = start;
	endColor = end;
}


//----------------------------------------------------------------------------------------------------------------
void ParticleEmitter::SetLifetime( float min, float max ) {
	particleLifespan = FloatRange(min, max);
}


//----------------------------------------------------------------------------------------------------------------
void ParticleEmitter::SetBurstAmount( int min, int max ) {
	burstSize = IntRange(min, max);
}


void ParticleEmitter::SpawnBurst() {
	int currentBurstSize = burstSize.GetRandomInRange();
	for (int i = 0; i < currentBurstSize; i++) {
		SpawnParticle();
	}
}


//----------------------------------------------------------------------------------------------------------------
bool ParticleEmitter::IsSafeToDestroy() const {
	return (spawnRate == 0) && (particles.size() == 0);
}


//----------------------------------------------------------------------------------------------------------------
void ParticleEmitter::DefaultUpdate( ParticleEmitter* pe ) {

	if (pe->clock->total.seconds - pe->timeAtLastSpawn > pe->spawnRate) {
		int timesElapsed = (int) ((pe->clock->total.seconds - pe->timeAtLastSpawn) / pe->spawnRate);
		for (int i = 0; i < timesElapsed; i++) {
			pe->SpawnParticle();
		}
		pe->timeAtLastSpawn = pe->clock->total.seconds;
	}

	for (int i = 0; i < pe->particles.size(); i++) {
		Particle* p = &(pe->particles[i]);
		p->force = Vector3(0.f, -1.f, 0.f);
		p->Update(p, pe->clock->frame.seconds);
	}

	for (int index = (int) pe->particles.size() - 1; index >= 0; index--) {
		if (pe->particles[index].IsDead( pe->clock->total.seconds )) {
			pe->particles[index] = pe->particles[pe->particles.size() - 1];
			pe->particles.pop_back();
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void ParticleEmitter::DefaultPreRender( ParticleEmitter* pe, Camera* camera ) {

	if (pe->mesh != nullptr) {
		delete pe->mesh;
	}
	pe->mesh = new Mesh();
	MeshBuilder builder;
	builder.Begin(TRIANGLES, false);
	Matrix44 cameraModel = camera->transform.GetLocalToWorldMatrix();
	Matrix44 particleModel = pe->transform.GetWorldToLocalMatrix();
	particleModel.Append(cameraModel);

	Vector3 up = particleModel.GetUp();
	Vector3 right = particleModel.GetRight();

	for (int particleIndex = 0; particleIndex < pe->particles.size(); particleIndex++) {
		Particle* p = &(pe->particles[particleIndex]);

		Vector3 bl = p->position - (up * p->size) - (right * p->size);
		Vector3 br = p->position - (up * p->size) + (right * p->size);
		Vector3 tr = p->position + (up * p->size) + (right * p->size);
		Vector3 tl = p->position + (up * p->size) - (right * p->size);
		Rgba color = Interpolate( pe->startColor, pe->endColor, p->GetNormalizedAge( pe->clock->total.seconds ) );

		builder.SetColor(color);
		builder.PushQuad(bl, br, tr, tl);
	}

	builder.End();
	pe->mesh->FromBuilderAsType<Vertex3D_PCU>(&builder);
	pe->renderable->SetMesh(pe->mesh);
	pe->renderable->SetModelMatrix(pe->transform.GetLocalToWorldMatrix());
}