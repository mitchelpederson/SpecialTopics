#include "Engine/Core/Transform.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

Transform const Transform::IDENTITY = Transform(nullptr, Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f), Vector3(1.f, 1.f, 1.f));


Transform::Transform( Transform* par, const Vector3& pos /* = Vector3() */, const Vector3& eul /* = Vector3() */, const Vector3& scl /* = Vector3() */) 
	: position( pos )
	, euler( eul )
	, scale( scl ) 
	, parent( par )
{}

Matrix44 Transform::GetLocalToWorldMatrix() const {
	Matrix44 scaleMat = Matrix44::MakeScale(scale);
	Matrix44 rotMat = Matrix44::MakeRotationDegrees(euler);
	Matrix44 transMat = Matrix44::MakeTranslation(position);

	transMat.Append(rotMat);
	transMat.Append(scaleMat);
	if (parent != nullptr) {
		Matrix44 parentModel = parent->GetLocalToWorldMatrix();
		parentModel.Append(transMat);
		return parentModel;
	}
	else {
		return transMat;
	}
	
}


Matrix44 Transform::GetLocalToParentMatrix() const {
	Matrix44 scaleMat = Matrix44::MakeScale(scale);
	Matrix44 rotMat = Matrix44::MakeRotationDegrees(euler);
	Matrix44 transMat = Matrix44::MakeTranslation(position);

	transMat.Append(rotMat);
	transMat.Append(scaleMat);
	return transMat;
}


Matrix44 Transform::GetWorldToLocalMatrix() const {
	Matrix44 ltow = GetLocalToWorldMatrix();
	return ltow.GetInverse();
}


Matrix44 Transform::GetParentToLocalMatrix() const {
	Matrix44 ptow = GetLocalToParentMatrix();
	return ptow.GetInverse();
}


Vector3 Transform::GetWorldLocation() const {
	return GetLocalToWorldMatrix().TransformPosition(position);
}

Vector3 Transform::GetWorldForward() const {
	return GetLocalToWorldMatrix().GetForward();
}

Vector3 Transform::GetWorldUp() const {
	return GetLocalToWorldMatrix().GetUp();
}

Vector3 Transform::GetWorldRight() const {
	return GetLocalToWorldMatrix().GetRight();
}

void Transform::Translate( const Vector3& translation ) {
	position = position + translation;
}


void Transform::Rotate( const Vector3& rotationEuler ) {
	Matrix44 srt = GetLocalToWorldMatrix();
	Matrix44 rot = Matrix44::MakeRotationDegrees(rotationEuler);
	srt.Append(rot);
	euler = srt.GetRotation();
}


void Transform::Scale( const Vector3& scaleFactors ) {
	scale.x *= scaleFactors.x;
	scale.y *= scaleFactors.y;
	scale.z *= scaleFactors.z;
}


void Transform::Scale( float scaleFactor ) {
	scale = scale * scaleFactor;
}


void Transform::AlignToNewUp( const Vector3& up ) {
	Matrix44 oldModelMatrix = GetLocalToWorldMatrix();
	Vector3 planeRight = Vector3::CrossProduct(Vector3::UP, oldModelMatrix.GetForward());
	Vector3 newForward = Vector3::CrossProduct(planeRight, up);
	Vector3 newRight = Vector3::CrossProduct(up, newForward);
	
	Matrix44 newRotation( newRight, up, newForward );

	euler = newRotation.GetRotation();
}


void Transform::LookToward( const Vector3& forward, const Vector3& worldUp /* = Vector3::UP */ ) {
	Vector3 newForward = forward.GetNormalized();
	Vector3 newRight = Vector3::CrossProduct(worldUp, newForward);
	newRight.Normalize();
	Vector3 newUp = Vector3::CrossProduct(newForward, newRight);
	newUp.Normalize();

	Matrix44 newRotation(newRight, newUp, newForward);
	euler = newRotation.GetRotation();
}


Matrix44 Transform::LookAtLocal( const Vector3& localTarget, const Vector3& localUp /* = Vector3::UP */ ) {

	Vector3 direction = localTarget - position;
	Vector3 newForward = direction.GetNormalized();
	Vector3 newRight = Vector3::CrossProduct(localUp, newForward);
	newRight.Normalize();
	Vector3 newUp = Vector3::CrossProduct(newForward, newRight);
	newUp.Normalize();

	Matrix44 newRotation(newRight, newUp, newForward);
	//euler = newRotation.GetRotation();

	return newRotation;

}


void Transform::LookAtWorld( const Vector3& worldPosition, const Vector3& worldUp /* = Vector3::UP */ ) {
	Vector3 transformedWorldPosition = GetWorldToLocalMatrix().TransformPosition(worldPosition);
	Vector3 transformedWorldUp = GetWorldToLocalMatrix().TransformDirection(worldUp);
	LookAtLocal(transformedWorldPosition, transformedWorldUp);
}


void Transform::TurnToward( const Matrix44& current, const Matrix44& target, float maxDeltaDegrees ) {

	Matrix44 currentInverse = current.GetInverse();
	Matrix44 r = target;
	currentInverse.Append(r);

	float trace = currentInverse.GetTrace3();
	float inner = ClampFloatNegativeOneToOne((trace - 1.f) * 0.5f);
	float angle = AcosDegrees(inner);

	float percentToRotate = Min( maxDeltaDegrees / angle, 1.f );

	Matrix44 newRotation = LerpMatrix(current, target, percentToRotate);
	euler = newRotation.GetRotation();
}