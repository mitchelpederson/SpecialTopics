#pragma once
#include "Engine/Renderer/Sprites/Sprite.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

#include <map>

class EntityDefinition {
public:
	EntityDefinition( tinyxml2::XMLElement const& xml );

	static std::map<unsigned char, EntityDefinition*> s_definitions;

	unsigned char GetID() const;
	float	GetCosmeticRadius() const;
	float	GetPhysicalRadius() const;
	float	GetMaxHealth() const;
	float	GetMaxTurnRate() const;
	float	GetMaxMoveSpeed() const;
	float	GetShootingDelay() const;
	float	GetShotDamage() const;
	bool	IsSolid() const;
	bool	IsHostile() const;
	bool	Is2DSoundSource() const;
	std::string GetSpriteSetName() const;
	Rgba	GetMinimapColor() const;

private:
	unsigned char m_id;
	float		m_cosmeticRadius;
	float		m_physicalRadius;
	float		m_maxHealth;
	float		m_maxTurnRate;
	float		m_maxMoveSpeed;
	float		m_shootingDelay;
	float		m_shotDamage;
	bool		m_isSolid;
	bool		m_isHostile;
	bool		m_playSounds2D;
	std::string m_name;
	std::string	m_spriteSetName = "NONE";
	Rgba		m_minimapColor;
	
};