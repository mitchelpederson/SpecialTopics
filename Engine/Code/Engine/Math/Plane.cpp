#include "Engine/Math/Plane.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Plane::Plane( const Vector3& norm, float dist ) 
	: normal(norm)
	, distance(dist)
{
}


Plane::Plane( const Vector3& pos1, const Vector3& pos2, const Vector3& pos3 ) {

	Vector3 displacement1 = pos2 - pos1;
	Vector3 displacement2 = pos3 - pos1;

	normal = Vector3::CrossProduct(displacement1, displacement2);

	normal.Normalize();
	distance = DotProduct(normal, pos1);
}


float Plane::GetDistance( const Vector3& point ) const {
	float dist = DotProduct(point, normal);
	return dist - distance;
}


bool Plane::IsInFront( const Vector3& point ) const {
	return GetDistance(point) > 0.f;
}


void Plane::FlipNormal() {
	normal = normal * -1.f;
	distance *= -1.f;
}


RaycastHit3 Plane::CheckForRayCollision( const Ray3& ray ) const {

	float cosAngle = DotProduct(ray.direction, normal);

	// easy out if perpendicular
	if (cosAngle == 0.f ) {
		return RaycastHit3(false, Vector3::ZERO, INFINITY);
	}

	float distanceFromPlane = GetDistance( ray.position );
	// If the signs of these 
	if ((distanceFromPlane * cosAngle) > 0.f) {
		return RaycastHit3(false, Vector3::ZERO, INFINITY);
	}

	float time = -distanceFromPlane / cosAngle;
	return RaycastHit3( true, ray.Evaluate(time), distanceFromPlane );
}