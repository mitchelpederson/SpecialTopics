#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Texture.hpp"

class Light {

public:
	Light();
	void SetAsPointLight( const Vector3& position, const Rgba& color, float intensity = 1.f, float attenuation = 0.4f );
	void SetAsDirectionalLight( const Vector3& position, const Vector3& direction, const Rgba& color, float intensity = 1.f, float isShadowcasting = 0.f, float attenuation = 0.f);
	void SetAsSpotLight( const Vector3& position, const Vector3& direction, float innerAngle, float outerAngle, const Rgba& color, float intensity = 1.f, float attenuation = 0.f);
	void SetPosition( const Vector3& position );

	Texture* CreateOrGetShadowTexture();


public:
	Vector3 m_position = Vector3();
	Vector3 m_direction = Vector3();
	float m_innerAngle = 360.f;
	float m_outerAngle = 360.f;
	Rgba m_color = Rgba();
	float m_intensity = 1.f;
	float m_attenuation = 0.f;
	float m_isPointLight = 1.f;

	// Shadow data and rendering objects
	float m_isShadowcasting = 0.f;
	Transform m_transform;
	Matrix44 m_projection;
	Matrix44 m_viewProjection;
	Matrix44 m_inverseViewProjection;
	Texture* m_depthTarget = nullptr;
	IntVector2 m_shadowMapResolution = IntVector2(1024, 1024);
};