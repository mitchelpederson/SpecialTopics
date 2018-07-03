#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/AABB2.hpp"

class Matrix44 {
public:

	Matrix44();
	explicit Matrix44( float initialValues[16] );
	explicit Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f, 0.f));
	explicit Matrix44( const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation = Vector3(0.f, 0.f, 0.f));

	Matrix44 GetTranspose() const;
	Matrix44 GetInverseFast() const;
	Matrix44 GetInverse() const;

	Vector2 TransformDisplacement2D( const Vector2& displacement );
	Vector2 TransformPosition2D( const Vector2& point );
	Vector3 TransformPosition( const Vector3& point) const;
	Vector3 TransformDirection( const Vector3& direction) const;
	Vector4 Transform( const Vector4& coord ) const;
	
	void Append( const Matrix44& rightSide );
	void RotateDegrees2D( float degreesToRotate );
	void Scale2D( float scaleX, float scaleY );
	void ScaleUniform2D( float scale );
	void SetIdentity();
	void SetValues( float values[16] );
	void Translate2D( const Vector2& translation );
	void Invert();

	static Matrix44 MakeOrtho2D( const Vector2& mins, const Vector2& maxs );
	static Matrix44 MakeOrthographic( float left, float right, float top, float bottom, float far, float near);
	static Matrix44 MakeRotationDegrees2D( float degreesToRotate );
	static Matrix44 MakeScale2D( float scaleX, float scaleY );
	static Matrix44 MakeScaleUniform2D( float scale );
	static Matrix44 MakeTranslation2D( const Vector2& translation );
	static Matrix44 MakeProjection( float fov, float aspect, float zNear, float zFar );
	static Matrix44 MakeTranslation( const Vector3& translation );
	static Matrix44 MakeRotationDegrees( const Vector3& rotation );
	static Matrix44 MakeScale( const Vector3& scale );
	static Matrix44 MakeViewFromCamera( const Matrix44& cameraRot, const Vector3& cameraPos );

	Vector3 GetUp() const;
	Vector3 GetRight() const;
	Vector3 GetForward() const;
	Vector3 GetTranslation() const;
	Vector3 GetRotation() const;
	float GetTrace3() const;
	
	float Ix = 1.f;
	float Iy = 0.f;
	float Iz = 0.f;
	float Iw = 0.f;
	float Jx = 0.f;
	float Jy = 1.f;
	float Jz = 0.f;
	float Jw = 0.f;
	float Kx = 0.f;
	float Ky = 0.f;
	float Kz = 1.f;
	float Kw = 0.f;
	float Tx = 0.f;
	float Ty = 0.f;
	float Tz = 0.f;
	float Tw = 1.f;

};


Matrix44 InterpolateRotation( const Matrix44& start, const Matrix44& end, float fractionTowards );
Matrix44 LerpMatrix( const Matrix44& start, const Matrix44& end, float fractionTowards);