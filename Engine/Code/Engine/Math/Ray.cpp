#include "Engine/Math/Ray.hpp"


Ray3::Ray3() : position(Vector3::ZERO), direction(Vector3::FORWARD) 
{

}

Ray3::Ray3(const Vector3& position, const Vector3& direction)
	: position(position)
	, direction(direction)
{

}

Vector3 Ray3::Evaluate( float t ) const {
	return position + (direction * t);
}