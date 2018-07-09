#include "Engine/Renderer/Light.hpp"


Light::Light() {}


void Light::SetAsPointLight( const Vector3& position, const Rgba& color, float intensity /* = 1.f */, float attenuation /* = 0.4f */ ) {
	m_position = position;
	m_color = color;
	m_intensity = intensity;
	m_attenuation = attenuation;
	m_innerAngle = 360.f;
	m_outerAngle = 360.f;
	m_isPointLight = 1.f;
	m_isShadowcasting = 0.f;
	m_direction = Vector3::FORWARD;

}


void Light::SetAsDirectionalLight(const Vector3& position, const Vector3& direction, const Rgba& color, float intensity /* = 1.f */, float isShadowcasting, float attenuation /* = 0.f */) {
	m_position = position;
	m_color = color;
	m_intensity = intensity;
	m_attenuation = m_attenuation;
	m_direction = direction;
	m_innerAngle = 360.f;
	m_outerAngle = 360.f;
	m_isPointLight = 0.f;
	m_isShadowcasting = isShadowcasting;
	m_transform.position = position;
	m_transform.LookToward(direction);
}


void Light::SetAsSpotLight(const Vector3& position, const Vector3& direction, float innerAngle, float outerAngle, const Rgba& color, float intensity /* = 1.f */, float attenuation /* = 0.f */) {
	m_position = position;
	m_color = color;
	m_intensity = intensity;
	m_attenuation = m_attenuation;
	m_innerAngle = innerAngle;
	m_outerAngle = outerAngle;
	m_isPointLight = 1.f;
	m_isShadowcasting = 0.f;

}


void Light::SetPosition( const Vector3& position ) {
	m_position = position;
}


Texture* Light::CreateOrGetShadowTexture() {
	if (m_depthTarget == nullptr) {
		m_depthTarget = new Texture();
		m_depthTarget->SetSamplerMode(SAMPLER_SHADOW);
		m_depthTarget->CreateRenderTarget(m_shadowMapResolution.x, m_shadowMapResolution.y, TEXTURE_FORMAT_D24S8);
		return m_depthTarget;
	}
	else {
		return m_depthTarget;
	}
}