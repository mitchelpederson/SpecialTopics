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
}


void Light::SetAsDirectionalLight(const Vector3& position, const Vector3& direction, const Rgba& color, float intensity /* = 1.f */, float attenuation /* = 0.f */) {
	m_position = position;
	m_color = color;
	m_intensity = intensity;
	m_attenuation = m_attenuation;
	m_direction = direction;
	m_innerAngle = 360.f;
	m_outerAngle = 360.f;
	m_isPointLight = 0.f;
}


void Light::SetAsSpotLight(const Vector3& position, const Vector3& direction, float innerAngle, float outerAngle, const Rgba& color, float intensity /* = 1.f */, float attenuation /* = 0.f */) {
	m_position = position;
	m_color = color;
	m_intensity = intensity;
	m_attenuation = m_attenuation;
	m_innerAngle = innerAngle;
	m_outerAngle = outerAngle;
	m_isPointLight = 1.f;
}


void Light::SetPosition( const Vector3& position ) {
	m_position = position;
}