#pragma once
#include "Engine/Math/Matrix44.hpp"


class Transform {
public:
	Transform( Transform* parent = nullptr, const Vector3& pos = Vector3(), const Vector3& eul = Vector3(), const Vector3& scl = Vector3(1.f, 1.f, 1.f));

	Matrix44 GetLocalToWorldMatrix() const;
	Matrix44 GetLocalToParentMatrix() const;
	Matrix44 GetWorldToLocalMatrix() const;
	Matrix44 GetParentToLocalMatrix() const;
	Vector3 GetWorldLocation() const;
	Vector3 GetWorldForward() const;
	Vector3 GetWorldUp() const;
	Vector3 GetWorldRight() const;

	void Translate( const Vector3& translation );
	void Rotate( const Vector3& rotationEuler );
	void Scale( const Vector3& scaleFactors );
	void Scale( float scaleFactor );

	void AlignToNewUp( const Vector3& up );
	Matrix44 LookAtLocal( const Vector3& localPosition, const Vector3& localUp = Vector3::UP );
	void LookAtWorld( const Vector3& worldPosition, const Vector3& worldUp = Vector3::UP );
	void TurnToward( const Matrix44& current, const Matrix44& target, float maxDeltaDegrees );
	void LookToward( const Vector3& forward, const Vector3& worldUp = Vector3::UP );

	static Transform const IDENTITY;
public:
	Vector3 position = Vector3();
	Vector3 euler = Vector3();
	Vector3 scale = Vector3(1.f, 1.f, 1.f);

	Transform* parent = nullptr;
};