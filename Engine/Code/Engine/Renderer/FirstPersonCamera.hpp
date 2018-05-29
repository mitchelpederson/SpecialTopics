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
	void Update(float deltaSeconds) override;

public:
	float aspectRatio = 1.6667f;
	float nearZ = 1.f;
	float farZ = 1000.f;

};

