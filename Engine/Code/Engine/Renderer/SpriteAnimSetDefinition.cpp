#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

SpriteAnimSetDefinition::SpriteAnimSetDefinition( const tinyxml2::XMLElement& xml ) {

	std::string spriteSheetPath = "ERROR";
	IntVector2 spriteLayout(1, 1);

	spriteSheetPath = ParseXmlAttribute(xml, "spriteSheet", spriteSheetPath);
	spriteLayout = ParseXmlAttribute(xml, "spriteLayout", spriteLayout);
	GUARANTEE_OR_DIE(spriteSheetPath != "ERROR", "spriteSheet image does not exist");
	SpriteSheet* spriteSheet = new SpriteSheet(g_theRenderer->CreateOrGetTexture(spriteSheetPath), spriteLayout);

	int defaultFPS = -1;
	defaultFPS = ParseXmlAttribute(xml, "fps", defaultFPS);

	const tinyxml2::XMLElement* spriteAnimElement = xml.FirstChildElement();
	while (spriteAnimElement != nullptr) {

		SpriteAnimDefinition* animDef = new SpriteAnimDefinition(*spriteAnimElement, defaultFPS);

		animDef->m_spriteSheet = spriteSheet;

		std::string animName = "ERROR";
		animName = ParseXmlAttribute(*spriteAnimElement, "name", animName);
		GUARANTEE_OR_DIE(animName != "ERROR", "SpriteAnim did not have a valid name");

		m_animDefs[animName] = animDef;

		spriteAnimElement = spriteAnimElement->NextSiblingElement();
	}

	m_defaultAnimName = ParseXmlAttribute(xml, "defaultAnim", m_defaultAnimName);
	m_additiveBlend = ParseXmlAttribute(xml, "additiveBlend", m_additiveBlend);

	GUARANTEE_OR_DIE(m_animDefs.find(m_defaultAnimName) != m_animDefs.end(), "Default animation does not exist!");
}


SpriteAnimSetDefinition::~SpriteAnimSetDefinition() {

	std::map<std::string, SpriteAnimDefinition*>::iterator anim = m_animDefs.begin();
	while (anim != m_animDefs.end()) {
		delete m_animDefs[anim->first];
		m_animDefs[anim->first] = nullptr;
		anim++;
	}
}


bool SpriteAnimSetDefinition::IsAdditiveBlending() const {
	return m_additiveBlend;
}