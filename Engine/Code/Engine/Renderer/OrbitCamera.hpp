#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"

class OrbitCamera : public Camera{
public:

	OrbitCamera();
	~OrbitCamera();

	Matrix44 GetProjectionMatrix();
	Matrix44 GetCameraMatrix();
	Matrix44 GetViewMatrix();
	void Update() override;

public:
	float orthoSize;
	float aspectRatio = 1.6667f;
	float nearZ = 1.f;
	float farZ = 1000.f;

	Vector3 target;
	float radius = 3.f;
	float rotation = 1.f;
	float angle = 1.f;
};

