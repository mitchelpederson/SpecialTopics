#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Core/EngineCommon.hpp"


OrbitCamera::OrbitCamera() 
	: target(0.f, 0.f, 0.f)
{

}

void OrbitCamera::Update() {
	Vector3 position = target + Vector3(-radius * CosDegrees(rotation) * CosDegrees(angle), radius * SinDegrees(angle), -radius * CosDegrees(angle) * SinDegrees(rotation));
	LookAt(position, target, Vector3::UP);

	m_cameraMatrix = transform.GetLocalToWorldMatrix();
	m_viewMatrix = m_cameraMatrix.GetInverse();
	if (skybox != nullptr) {
		skybox->SetModelMatrix(m_cameraMatrix);
	}
}


Matrix44 OrbitCamera::GetProjectionMatrix() {
	return m_projMatrix;
}


Matrix44 OrbitCamera::GetCameraMatrix() {
	return m_cameraMatrix;
}


Matrix44 OrbitCamera::GetViewMatrix() {
	return m_viewMatrix;
}
