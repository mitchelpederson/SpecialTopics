#pragma once
#include "Engine/Math/Matrix44.hpp"


class Transform {
public:
	Transform( Transform* parent = nullptr, const Vector3& pos = Vector3(), const Vector3& eul = Vector3(), const Vector3& scl = Vector3(1.f, 1.f, 1.f));

	Matrix44 GetLocalToWorldMatrix();
	Matrix44 GetWorldToLocalMatrix();

	void Translate( const Vector3& translation );
	void Rotate( const Vector3& rotationEuler );
	void Scale( const Vector3& scaleFactors );
	void Scale( float scaleFactor );

	static Transform const IDENTITY;
public:
	Vector3 position;
	Vector3 euler;
	Vector3 scale;

	Transform* parent;
};