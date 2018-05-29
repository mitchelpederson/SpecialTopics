#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Core/EngineCommon.hpp"


OrbitCamera::OrbitCamera() 
	: target(0.f, 0.f, 0.f)
{

}

void OrbitCamera::Update(float deltaSeconds) {
	Vector3 position = target + Vector3(-radius * CosDegrees(rotation) * CosDegrees(angle), radius * SinDegrees(angle), -radius * CosDegrees(angle) * SinDegrees(rotation));
	LookAt(position, target, Vector3::UP);
}