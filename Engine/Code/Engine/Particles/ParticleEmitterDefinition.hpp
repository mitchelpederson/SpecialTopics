#pragma once

#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

#include <map>


enum eParticleEmitterType {
	PARTICLE_EMITTER_WORLD_PARTICLE,
	PARTICLE_EMITTER_LOCAL_PARTICLE,
	PARTICLE_EMITTER_RIBBON
};

class ParticleEmitterDefinition {

public:
	ParticleEmitterDefinition( const tinyxml2::XMLElement& xml );

	static const ParticleEmitterDefinition& GetDef( const std::string& name );
	static std::map< std::string, ParticleEmitterDefinition* > s_defs;


private:
	std::string m_name;
	std::string m_materialName;
	eParticleEmitterType m_type;

	float m_spawnRate;
	float m_spawnConeAngle;
	IntRange m_burstSize;

	FloatRange m_particleLifespan;
	FloatRange m_particleSpeedOnSpawn;
	FloatRange m_particleSize;
	Vector3 m_particleAcceleration;


	

};