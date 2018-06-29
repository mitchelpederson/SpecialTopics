#pragma once
#include "Engine/Math/Vector3.hpp"

struct RaycastHit3 {

	RaycastHit3(bool h, Vector3 pos, float dist) 
		: hit(h)
		, position(pos)
		, normal(Vector3::FORWARD)
		, distance(dist)
	{}

	bool hit;
	Vector3 position;
	Vector3 normal;
	float distance;
};


class Ray3 {

public:
	Ray3();
	Ray3(const Vector3& position, const Vector3& direction);

	Vector3 Evaluate( float t ) const;


public:
	Vector3 position;
	Vector3 direction;

};