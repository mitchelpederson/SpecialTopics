#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include <map>


class Sprite {

public:
	Sprite( const tinyxml2::XMLElement& xml );
	Sprite( const Sprite& copy );		// Copies are not registered in the static map

	static std::map<std::string, Sprite*> s_sprites;

	float GetPPU() const;
	AABB2 GetUVs() const;
	Texture* GetTexture() const;
	Vector2 GetDimensions() const;
	Vector2 GetPivot() const;
	Vector2 GetScale() const;

	void SetScale( const Vector2& scale );

private:
	Vector2 m_dimensions;
	Texture* m_spriteSheet;
	AABB2 m_uvs;
	Vector2 m_pivot;
	float m_ppu;
	Vector2 m_scale = Vector2(1.f, 1.f);

};