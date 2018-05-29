#include "Engine/Core/Transform.hpp"
#include "Engine/Math/Matrix44.hpp"


Transform const Transform::IDENTITY = Transform(nullptr, Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 0.f), Vector3(1.f, 1.f, 1.f));


Transform::Transform( Transform* par, const Vector3& pos /* = Vector3() */, const Vector3& eul /* = Vector3() */, const Vector3& scl /* = Vector3() */) 
	: position( pos )
	, euler( eul )
	, scale( scl ) 
	, parent( par )
{}

Matrix44 Transform::GetLocalToWorldMatrix() {
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


Matrix44 Transform::GetWorldToLocalMatrix() {
	Matrix44 ltow = GetLocalToWorldMatrix();
	return ltow.GetInverse();
}


void Transform::Translate( const Vector3& translation ) {
	position = position + translation;
}


void Transform::Rotate(  const Vector3& rotationEuler ) {
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