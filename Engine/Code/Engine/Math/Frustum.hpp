#pragma once
#include "Engine/Math/Plane.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vector3.hpp"

class Frustum {
public:
	Frustum();
	Plane left;
	Plane right;
	Plane up;
	Plane down;
	Plane near;
	Plane far;

	static Frustum FromMatrix( const Matrix44& mat );

	bool IsContained( const Vector3& point );
};