#pragma once
#include "Engine/Math/Vector3.hpp"


class Vector4 {
public:
	Vector4(float X, float Y, float Z, float W);
	Vector4();
	Vector4(const Vector3& xyz, float w);

	Vector4 GetNormalized() const;
	Vector3 xyz() const;

	static Vector4 CrossProduct( const Vector4& a, const Vector4& b );
	float NormalizeAndGetLength();
	float GetLength() const;
	void Normalize();

	const Vector4 operator*( float c ) const;
	const Vector4 operator-(const Vector4& b) const;
	const Vector4 operator+(const Vector4& b) const;

	static const Vector4 UP;
	static const Vector4 FORWARD;
	static const Vector4 RIGHT;
	static const Vector4 ZERO;
	static const Vector4 NORTH;
	static const Vector4 SOUTH;
	static const Vector4 EAST;
	static const Vector4 WEST;

public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;
};