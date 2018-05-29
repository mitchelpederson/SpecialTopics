#pragma once
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"

class SpriteAnimSet {

	friend class SpriteAnimSetDefinition;

public:
	SpriteAnimSet( const SpriteAnimSetDefinition* definition );
	~SpriteAnimSet();

	void Update( float deltaSeconds );
	void StartAnim( const std::string& name );
	void SetCurrentAnim( const std::string& name );
	SpriteAnim* GetSpriteAnim( const std::string& name );
	
	const	Texture&	GetCurrentTexture() const;
			AABB2		GetCurrentUVs() const;
			bool		IsAdditiveBlending() const;

private:

	const SpriteAnimSetDefinition*		m_definition;
	std::map<std::string, SpriteAnim*>	m_animations;
	SpriteAnim*							m_currentAnim;
	SpriteAnim*							m_previousAnim;

};