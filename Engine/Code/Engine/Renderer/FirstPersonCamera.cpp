#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Core/EngineCommon.hpp"

FirstPersonCamera::FirstPersonCamera() {

}

FirstPersonCamera::~FirstPersonCamera() {

}


Matrix44 FirstPersonCamera::GetProjectionMatrix() {
	return m_projMatrix;
}


Matrix44 FirstPersonCamera::GetCameraMatrix() {
	return m_cameraMatrix;
}


Matrix44 FirstPersonCamera::GetViewMatrix() {
	return m_viewMatrix;
}


void FirstPersonCamera::TranslateRelative( const Vector3& localOffset ) {

	Vector3 worldOffset = (GetRight() * localOffset.x) + (GetUp() * localOffset.y) + (GetForward() * localOffset.z); 
	transform.position = transform.position + worldOffset;

}


void FirstPersonCamera::RotateRelative( float pitchDegrees, float yawDegrees ) {
	transform.euler.x += pitchDegrees;
	transform.euler.y += yawDegrees;
}


void FirstPersonCamera::Update( float deltaSeconds ) {
	m_cameraMatrix = transform.GetLocalToWorldMatrix();
	m_viewMatrix = m_cameraMatrix.GetInverse();

	skybox->SetModelMatrix(m_cameraMatrix);
}