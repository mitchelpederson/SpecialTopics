#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Math/Plane.hpp"

#include <vector>

class AABB3 {

public:
	AABB3();
	AABB3( const Vector3& mins, const Vector3& maxs );

	Vector3 GetCenter() const;
	Vector3 GetDimensions() const;

	bool Contains( const Vector3& point ) const;
	std::vector<RaycastHit3> DoesRayIntersect( const Ray3& ray ) const;
	bool DoAABB3sOverlap( const AABB3& other ) const;

	Plane GetFrontPlane() const;
	Plane GetBackPlane() const;
	Plane GetLeftPlane() const;
	Plane GetRightPlane() const;
	Plane GetTopPlane() const;
	Plane GetBottomPlane() const;

	Vector3 GetNearestPointOnSurface( const Vector3& outsidePoint );

public:
	Vector3 mins;
	Vector3 maxs;
};