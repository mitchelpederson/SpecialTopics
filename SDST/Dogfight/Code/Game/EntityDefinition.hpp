#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

#include <map>


enum eFlightType {
	FLIGHT_PLANE,
	FLIGHT_MISSILE,
	FLIGHT_DUMB
};

class EntityDefinition {
public:
	EntityDefinition( const tinyxml2::XMLElement& xml );


public:

	static std::map< int, EntityDefinition* > s_definitions;

	const	std::string&	GetName() const;
	const	std::string&	GetMeshPath() const;
	const	std::string&	GetMaterialName() const;
	const	FloatRange&		GetThrustRange() const;
	const	FloatRange&		GetCrossSectionArea() const;
	const	FloatRange&		GetDragCoefficient() const;
	const	Rgba&			GetTrailColor() const;

			int				GetID() const;
			float			GetLifespan() const;
			float			GetMaxHealth() const;
			float			GetMaxVelocity() const;
			float			GetStallSpeed() const;
			float			GetMass() const;
			float			GetPhysicalRadius() const;
			float			GetRollDrag() const;
			float			GetYawDrag() const;
			float			GetPitchDrag() const;
			float			GetMaxRollSpeed() const;
			float			GetTrailLifespan() const;
			bool			CanReceiveLock() const;
			bool			IsWeapon() const;
			eFlightType		GetFlightStyle() const;


private:
	int			m_id = 0;
	float		m_lifespan = 5.f;
	float		m_maxHealth = 1.f;
	bool		m_canReceiveLock = true;
	std::string m_name = "ERROR";
	eFlightType	m_flightStyle = FLIGHT_PLANE;
	
	std::string m_meshPath = "ERROR";
	std::string m_materialName = "ERROR";
	FloatRange	m_thrustForceRange = FloatRange( 1.f, 1.f ); // newtons
	FloatRange	m_crossSectionArea = FloatRange( 1.f, 1.f ); // m^2
	FloatRange	m_dragCoefficient = 0.02f;
	float		m_maxVelocity = 1.f; // m/s
	float		m_stallSpeed = 0.f; // m/s
	float		m_mass = 1.f; // newtons
	float		m_physicalRadius = 1.f; // m

	float		m_rollDrag = 0.8f;
	float		m_yawDrag = 1.f;
	float		m_pitchDrag = 1.2f;
	float		m_maxRollSpeed = 50.f;

	Rgba		m_trailColor = Rgba();
	float		m_trailLifespan = 2.f;\
	bool		m_isWeapon = false;


};