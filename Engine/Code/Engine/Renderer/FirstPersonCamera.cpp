#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Core/EngineCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
FirstPersonCamera::FirstPersonCamera() : Camera() {

}


//----------------------------------------------------------------------------------------------------------------
FirstPersonCamera::~FirstPersonCamera() {

}


//----------------------------------------------------------------------------------------------------------------
Matrix44 FirstPersonCamera::GetProjectionMatrix() {
	return m_projMatrix;
}


//----------------------------------------------------------------------------------------------------------------
Matrix44 FirstPersonCamera::GetCameraMatrix() {
	return m_cameraMatrix;
}


//----------------------------------------------------------------------------------------------------------------
Matrix44 FirstPersonCamera::GetViewMatrix() {
	return m_viewMatrix;
}


//----------------------------------------------------------------------------------------------------------------
void FirstPersonCamera::TranslateRelative( const Vector3& localOffset ) {

	Vector3 worldOffset = (GetRight() * localOffset.x) + (GetUp() * localOffset.y) + (GetForward() * localOffset.z); 
	transform.position = transform.position + worldOffset;

}


//----------------------------------------------------------------------------------------------------------------
void FirstPersonCamera::RotateRelative( float pitchDegrees, float yawDegrees ) {
	transform.euler.x += pitchDegrees;
	transform.euler.y += yawDegrees;
}


//----------------------------------------------------------------------------------------------------------------
void FirstPersonCamera::ShakeCamera( float startIntensity, float duration ) {
	maxShakeIntensity = startIntensity;
	shakeDuration = duration;
	timeAtShakeStart = g_masterClock->total.seconds;
}


//----------------------------------------------------------------------------------------------------------------
void FirstPersonCamera::Update() {

	float normalizedTimeIntoShake;
	if ( shakeDuration > 0.f ) {
		normalizedTimeIntoShake = (g_masterClock->total.seconds - timeAtShakeStart) / shakeDuration;
		normalizedTimeIntoShake = ClampFloatZeroToOne( normalizedTimeIntoShake );
	}
	else {
		normalizedTimeIntoShake = 1.f;
	}

	Vector3 shakeDirection = GetRandomDirectionInCone(360.f);
	Vector3 shakeDisplacement = shakeDirection * Interpolate( maxShakeIntensity, 0.f, SmoothStop2( normalizedTimeIntoShake ) );

	Matrix44 camMatrix = transform.GetLocalToWorldMatrix();
	camMatrix.Append( Matrix44::MakeTranslation( shakeDisplacement ) );

	m_cameraMatrix = camMatrix;
	m_viewMatrix = m_cameraMatrix.GetInverse();

	if (skybox != nullptr) {
		skybox->SetModelMatrix(m_cameraMatrix);
	}
}