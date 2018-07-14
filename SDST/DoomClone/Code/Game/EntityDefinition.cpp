#include "Engine/Core/XmlUtilities.hpp"
#include "Game/EntityDefinition.hpp"

std::map<unsigned char, EntityDefinition*> EntityDefinition::s_definitions;


//----------------------------------------------------------------------------------------------------------------
EntityDefinition::EntityDefinition( tinyxml2::XMLElement const& xml ) {
	m_name				=					ParseXmlAttribute(xml, "name",				m_name);
	m_physicalRadius	=					ParseXmlAttribute(xml, "physicalRadius",	m_physicalRadius);
	m_maxHealth			=					ParseXmlAttribute(xml, "maxHealth",			m_maxHealth);
	m_id				= (unsigned char)	ParseXmlAttribute(xml, "id",				m_id);
	m_minimapColor		=					ParseXmlAttribute(xml, "minimapColor",		m_minimapColor);
	m_isHostile			=					ParseXmlAttribute(xml, "isHostile",			m_isHostile);
	m_shootingDelay		=					ParseXmlAttribute(xml, "shootingDelay",		m_shootingDelay);
	m_shotDamage		=					ParseXmlAttribute(xml, "shotDamage",		m_shotDamage);
	m_playSounds2D		=					ParseXmlAttribute(xml, "sounds2D",			m_playSounds2D);

	// Read the <sprite> element if it exists
	const tinyxml2::XMLElement* spriteElement = xml.FirstChildElement("sprite");
	if (spriteElement != nullptr) {
		m_spriteSetName = ParseXmlAttribute(*spriteElement, "name", m_spriteSetName);
	}

	// Read the <physics> element if it exists
	const tinyxml2::XMLElement* physicsElement = xml.FirstChildElement("physics");
	if (physicsElement != nullptr) {
		m_maxTurnRate = ParseXmlAttribute(*physicsElement, "maxTurnRate", m_maxTurnRate);
		m_maxMoveSpeed = ParseXmlAttribute(*physicsElement, "maxMoveSpeed", m_maxMoveSpeed);
		m_isSolid = ParseXmlAttribute(*physicsElement, "isSolid", m_isSolid);
	}

	s_definitions[m_id] = this;

}


//----------------------------------------------------------------------------------------------------------------
unsigned char EntityDefinition::GetID() const {
	return m_id;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetCosmeticRadius() const {
	return m_cosmeticRadius;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetPhysicalRadius() const {
	return m_physicalRadius;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetMaxHealth() const {
	return m_maxHealth;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetMaxTurnRate() const {
	return m_maxTurnRate;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetMaxMoveSpeed() const {
	return m_maxMoveSpeed;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetShootingDelay() const {
	return m_shootingDelay;
}


//----------------------------------------------------------------------------------------------------------------
float EntityDefinition::GetShotDamage() const {
	return m_shotDamage;
}


//----------------------------------------------------------------------------------------------------------------
bool EntityDefinition::IsSolid() const {
	return m_isSolid;
}


//----------------------------------------------------------------------------------------------------------------
bool EntityDefinition::IsHostile() const {
	return m_isHostile;
}


//----------------------------------------------------------------------------------------------------------------
std::string EntityDefinition::GetSpriteSetName() const {
	return m_spriteSetName;
}


//----------------------------------------------------------------------------------------------------------------
Rgba EntityDefinition::GetMinimapColor() const {
	return m_minimapColor;
}


//----------------------------------------------------------------------------------------------------------------
bool EntityDefinition::Is2DSoundSource() const {
	return m_playSounds2D;
}