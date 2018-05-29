#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

const Vector3 Vector3::UP		= Vector3( 0.f,  1.f,  0.f);
const Vector3 Vector3::FORWARD	= Vector3( 0.f,  0.f,  1.f);
const Vector3 Vector3::RIGHT	= Vector3( 1.f,  0.f,  0.f);
const Vector3 Vector3::ZERO		= Vector3( 0.f,  0.f,  0.f);
const Vector3 Vector3::NORTH	= Vector3( 0.f,  0.f,  1.f);
const Vector3 Vector3::SOUTH	= Vector3( 0.f,  0.f, -1.f);
const Vector3 Vector3::WEST		= Vector3( 1.f,  0.f,  0.f);
const Vector3 Vector3::EAST		= Vector3(-1.f,  0.f,  0.f);

Vector3::Vector3() {

}


Vector3::Vector3(float X, float Y, float Z)
	: x(X)
	, y(Y)
	, z(Z) 
{}


Vector3::Vector3(const Vector2& xy, float Z) 
	: x(xy.x)
	, y(xy.y)
	, z(Z)
{}


Vector3 Vector3::CrossProduct( const Vector3& a, const Vector3& b ) {
	return Vector3( (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (b.z * a.x), (a.x * b.y) - (a.y * b.x) );
}


const Vector3 Vector3::operator*( float c ) const {
	return Vector3( this->x * c, this->y * c, this->z * c );
}


const Vector3 Vector3::operator-( const Vector3& b ) const {
	return Vector3( this->x - b.x, this->y - b.y, this->z - b.z);
}

const Vector3 Vector3::operator+( const Vector3& b ) const {
	return Vector3( this->x + b.x, this->y + b.y, this->z + b.z);
}

const Vector3& Vector3::operator*=( float scale )  {
	*this = Vector3( x * scale, y * scale, z * scale );
	return *this;
}

Vector3 Vector3::GetNormalized() const {
	float length = GetLength();
	return Vector3(x / length, y / length, z / length);
}

float Vector3::GetLength() const {

	return sqrtf((x * x) + (y * y) + (z * z));
}

float Vector3::GetLengthSquared() const {

	return (x * x) + (y * y) + (z * z);
}

float Vector3::DistanceFrom( const Vector3& otherPoint ) const {
	return (*this - otherPoint).GetLength();
}

void Vector3::Normalize() {
	float length = GetLength();
	x = x / length;
	y = y / length;
	z = z / length;
}

float Vector3::NormalizeAndGetLength() {
	float length = GetLength();
	Normalize();
	return length;
}


void Vector3::RotateAroundPointByDegreesXZPlane( const Vector3& point, float angleDegrees ) {
	
	Vector2 pointXZ( point.x, point.z );
	Vector2 thisXZ( x, z );

	Vector2 displacement = pointXZ - thisXZ;
	float length = displacement.GetLength();
	float angleBetween = displacement.GetOrientationDegrees();
	float newAngleBetween = angleBetween + angleDegrees;
	Vector2 newDisplacement = Vector2::MakeDirectionAtDegrees(newAngleBetween) * length;
	//Vector2 actualDisplacement = newDisplacement - displacement;
	x = newDisplacement.x;
	z = newDisplacement.y;
}


Vector3 Interpolate(const Vector3& start, const Vector3& end, float fractionToward) {
	return Vector3(Interpolate(start.x, end.x, fractionToward)
		, Interpolate(start.y, end.y, fractionToward)
		, Interpolate(start.z, end.z, fractionToward));
}