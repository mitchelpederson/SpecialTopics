#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Ray.hpp"

class Plane {

public:
	Plane( const Vector3& pos1, const Vector3& pos2, const Vector3& pos3 );
	Plane( const Vector3& norm, float dist );

	float GetDistance( const Vector3& point ) const;
	bool IsInFront( const Vector3& point ) const;
	RaycastHit3 CheckForRayCollision( const Ray3& ray ) const;

	void FlipNormal();


public:
	Vector3 normal;
	float distance;

};
