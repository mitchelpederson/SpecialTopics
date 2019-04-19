#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

class FirstPersonCamera : public Camera{
public:

	FirstPersonCamera();
	~FirstPersonCamera();

	Matrix44 GetProjectionMatrix();
	Matrix44 GetCameraMatrix();
	Matrix44 GetViewMatrix();
	void TranslateRelative( const Vector3& translation );
	void RotateRelative( float pitchDegrees, float yawDegrees );
	void Update() override;

	void ShakeCamera( float startIntensity, float duration ); // intensity is the max translation in each direction

public:
	float aspectRatio = 1.6667f;
	float nearZ = 1.f;
	float farZ = 1000.f;

private:
	float maxShakeIntensity = 0.f;
	float shakeDuration = 0.f;
	float timeAtShakeStart = 0.f;

};

