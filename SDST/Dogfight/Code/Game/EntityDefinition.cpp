#include "Game/EntityDefinition.hpp"

#include "Engine/Core/XmlUtilities.hpp"


std::map< int, EntityDefinition* > EntityDefinition::s_definitions;


//----------------------------------------------------------------------------------------------------------------
EntityDefinition::EntityDefinition(  const tinyxml2::XMLElement& xml ) {
	m_id = ParseXmlAttribute( xml, "id", m_id );
	m_name = ParseXmlAttribute( xml, "name", m_name );

	const tinyxml2::XMLElement* gameplay = xml.FirstChildElement( "gameplay" );
	if ( gameplay != nullptr ) {
		m_maxHealth			= ParseXmlAttribute( *gameplay, "maxHealth",		m_maxHealth );
		m_physicalRadius	= ParseXmlAttribute( *gameplay, "collisionRadius",	m_physicalRadius );
		m_lifespan			= ParseXmlAttribute( *gameplay, "lifespan",			m_lifespan );
		m_canReceiveLock	= ParseXmlAttribute( *gameplay, "canReceiveLock",	m_canReceiveLock );
		m_isWeapon			= ParseXmlAttribute( *gameplay, "isWeapon",			m_isWeapon );

		std::string flightStyle = "error";
		flightStyle = ParseXmlAttribute( *gameplay, "flightStyle", flightStyle );
		if ( strcmp(flightStyle.c_str(), "plane") == 0 ) {
			m_flightStyle = FLIGHT_PLANE;
		} else if ( strcmp(flightStyle.c_str(), "missile") == 0  ) {
			m_flightStyle = FLIGHT_MISSILE;
		} else if ( strcmp(flightStyle.c_str(), "dumb") == 0 ) {
			m_flightStyle = FLIGHT_DUMB;
		}
	}

	const tinyxml2::XMLElement* visuals = xml.FirstChildElement( "visuals" );
	if ( visuals != nullptr ) {
		m_meshPath		= ParseXmlAttribute( *visuals, "meshPath",		m_meshPath );
		m_materialName	= ParseXmlAttribute( *visuals, "materialName",	m_materialName );
		m_trailColor	= ParseXmlAttribute( *visuals, "trailColor",	m_trailColor );
		m_trailLifespan = ParseXmlAttribute( *visuals, "trailLifespan", m_trailLifespan );
	}

	const tinyxml2::XMLElement* physics = xml.FirstChildElement( "physics" );
	if ( physics != nullptr ) {
		m_thrustForceRange	= ParseXmlAttribute( *physics, "thrustRange",		m_thrustForceRange );
		m_maxVelocity		= ParseXmlAttribute( *physics, "maxVelocity",		m_maxVelocity );
		m_stallSpeed		= ParseXmlAttribute( *physics, "stallSpeed",		m_stallSpeed );
		m_mass				= ParseXmlAttribute( *physics, "mass",				m_mass );
		m_crossSectionArea	= ParseXmlAttribute( *physics, "crossSectionArea",	m_crossSectionArea );
		m_dragCoefficient	= ParseXmlAttribute( *physics, "dragCoefficient",	m_dragCoefficient );
		m_rollDrag			= ParseXmlAttribute( *physics, "rollDrag",			m_rollDrag );
		m_yawDrag			= ParseXmlAttribute( *physics, "yawDrag",			m_yawDrag );
		m_pitchDrag			= ParseXmlAttribute( *physics, "pitchDrag",			m_pitchDrag );
		m_maxRollSpeed		= ParseXmlAttribute( *physics, "maxRollSpeed",		m_maxRollSpeed );
	}

	s_definitions[ m_id ] = this;
}


//----------------------------------------------------------------------------------------------------------------
int EntityDefinition::GetID() const {
	return m_id;
}


//----------------------------------------------------------------------------------------------------------------
const std::string& EntityDefinition::GetName() const {
	return m_name;
}


//----------------------------------------------------------------------------------------------------------------
const std::string& EntityDefinition::GetMeshPath() const {
	return m_meshPath;
}


//----------------------------------------------------------------------------------------------------------------
const std::string& EntityDefinition::GetMaterialName() const {
	return m_materialName;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetMaxHealth() const {
	return m_maxHealth;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetMaxVelocity() const {
	return m_maxVelocity;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetStallSpeed() const {
	return m_stallSpeed;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetMass() const {
	return m_mass;
}




//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetPhysicalRadius() const {
	return m_physicalRadius;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetRollDrag() const {
	return m_rollDrag;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetYawDrag() const {
	return m_yawDrag;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetPitchDrag() const {
	return m_pitchDrag;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetMaxRollSpeed() const {
	return m_maxRollSpeed;
}


//----------------------------------------------------------------------------------------------------------------
const FloatRange& EntityDefinition::GetThrustRange() const {
	return m_thrustForceRange;
}


//----------------------------------------------------------------------------------------------------------------
const FloatRange& EntityDefinition::GetCrossSectionArea() const {
	return m_crossSectionArea;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetLifespan() const {
	return m_lifespan;
}


//----------------------------------------------------------------------------------------------------------------
bool EntityDefinition::CanReceiveLock() const {
	return m_canReceiveLock;
}


//----------------------------------------------------------------------------------------------------------------
eFlightType EntityDefinition::GetFlightStyle() const {
	return m_flightStyle;
}


//----------------------------------------------------------------------------------------------------------------
const Rgba& EntityDefinition::GetTrailColor() const {
	return m_trailColor;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetTrailLifespan() const {
	return m_trailLifespan;
}


//----------------------------------------------------------------------------------------------------------------
const FloatRange& EntityDefinition::GetDragCoefficient() const {
	return m_dragCoefficient;
}


//----------------------------------------------------------------------------------------------------------------
bool EntityDefinition::IsWeapon() const {
	return m_isWeapon;
}