#pragma once
#include "Engine/Math/Vector2.hpp"


class Vector3 {
public:
	Vector3(float X, float Y, float Z);
	Vector3();
	Vector3(const Vector2& xy, float Z);

	// Mutators
	static Vector3 CrossProduct( const Vector3& a, const Vector3& b );
	float NormalizeAndGetLength();
	void Normalize();
	void RotateAroundPointByDegreesXZPlane( const Vector3& point, float angleDegrees );

	// Queries
	Vector3 GetNormalized() const;
	float GetLength() const;
	float GetLengthSquared() const;
	float DistanceFrom( const Vector3& otherPoint ) const;

	// Operator Overloads
	const Vector3 operator*( float c ) const;
	const Vector3 operator-(const Vector3& b) const;
	const Vector3 operator+(const Vector3& b) const;
	const Vector3& operator*=(float scale);
	const Vector3& operator+=(const Vector3& b);
	friend bool operator==(const Vector3& a, const Vector3& b);

	static const Vector3 UP;
	static const Vector3 FORWARD;
	static const Vector3 RIGHT;
	static const Vector3 ZERO;
	static const Vector3 NORTH;
	static const Vector3 SOUTH;
	static const Vector3 EAST;
	static const Vector3 WEST;

public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};


Vector3 Interpolate(const Vector3& start, const Vector3& end, float fractionToward);
Vector3 Slerp(const Vector3& start, const Vector3& end, float fractionToward);
Vector3 SlerpUnitVectors(const Vector3& start, const Vector3& end, float fractionToward);
