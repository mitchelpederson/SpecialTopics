#include "Engine/Renderer/SpriteAnimSet.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"


SpriteAnimSet::SpriteAnimSet( const SpriteAnimSetDefinition* definition ) {
	
	m_definition = definition;
	std::map<std::string, SpriteAnimDefinition*>::const_iterator anim = m_definition->m_animDefs.begin();
	while (anim != m_definition->m_animDefs.end()) {
		m_animations[anim->first] = new SpriteAnim(anim->second);
		anim++;
	}

	m_currentAnim = m_animations[m_definition->m_defaultAnimName];
	m_previousAnim = m_currentAnim;
}


SpriteAnimSet::~SpriteAnimSet() {
	std::map<std::string, SpriteAnim*>::iterator anim = m_animations.begin();
	while (anim != m_animations.end()) {
		delete m_animations[anim->first];
		m_animations[anim->first] = nullptr;
		anim++;
	}
}


void SpriteAnimSet::Update( float deltaSeconds ) {
	m_currentAnim->Update(deltaSeconds);
	if (m_currentAnim->IsFinished()) {
		m_currentAnim = m_previousAnim;
	}
}


void SpriteAnimSet::StartAnim( const std::string& name ) {

	if (m_animations.find(name) != m_animations.end()) {
		m_previousAnim = m_currentAnim;
		m_currentAnim = m_animations[name];
	}
	//m_currentAnim->Reset();
}


const Texture& SpriteAnimSet::GetCurrentTexture() const {
	return *m_currentAnim->GetTexture();
}


AABB2 SpriteAnimSet::GetCurrentUVs() const {
	return m_currentAnim->GetCurrentTexCoords();
}

bool SpriteAnimSet::IsAdditiveBlending() const {
	return m_definition->IsAdditiveBlending();
}


SpriteAnim* SpriteAnimSet::GetSpriteAnim( const std::string& name ) {
	return m_animations[name];
}