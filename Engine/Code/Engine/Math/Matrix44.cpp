#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"



Matrix44::Matrix44() {

}


Matrix44::Matrix44( float initialValues[16] ) {
	Ix = initialValues[0];
	Iy = initialValues[1];
	Iz = initialValues[2];
	Iw = initialValues[3];
	Jx = initialValues[4];
	Jy = initialValues[5];
	Jz = initialValues[6];
	Jw = initialValues[7];
	Kx = initialValues[8];
	Ky = initialValues[9];
	Kz = initialValues[10];
	Kw = initialValues[11];
	Tx = initialValues[12];
	Ty = initialValues[13];
	Tz = initialValues[14];
	Tw = initialValues[15];
}


Matrix44::Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation ) {
	Ix = iBasis.x;
	Iy = iBasis.y;
	Jx = jBasis.x;
	Jy = jBasis.y;
	Tx = translation.x;
	Ty = translation.y;
}


Matrix44::Matrix44(const Vector3& iBasis, const Vector3& jBasis, const Vector3& kBasis, const Vector3& translation /* = Vector3(0.f, 0.f, 0.f) */) {
	Ix = iBasis.x;
	Iy = iBasis.y;
	Iz = iBasis.z;
	Jx = jBasis.x;
	Jy = jBasis.y;
	Jz = jBasis.z;
	Kx = kBasis.x;
	Ky = kBasis.y;
	Kz = kBasis.z;
	Tx = translation.x;
	Ty = translation.y;
	Tz = translation.z;
}


void Matrix44::SetValues( float values[16] ) {
	Ix = values[0];
	Iy = values[1];
	Iz = values[2];
	Iw = values[3];
	Jx = values[4];
	Jy = values[5];
	Jz = values[6];
	Jw = values[7];
	Kx = values[8];
	Ky = values[9];
	Kz = values[10];
	Kw = values[11];
	Tx = values[12];
	Ty = values[13];
	Tz = values[14];
	Tw = values[15];
}


void Matrix44::Append( const Matrix44& rightSide) {

	Matrix44 leftSide = *this;

	Ix = (leftSide.Ix * rightSide.Ix) + (leftSide.Jx * rightSide.Iy) + (leftSide.Kx * rightSide.Iz) + (leftSide.Tx * rightSide.Iw);
	Iy = (leftSide.Iy * rightSide.Ix) + (leftSide.Jy * rightSide.Iy) + (leftSide.Ky * rightSide.Iz) + (leftSide.Ty * rightSide.Iw);
	Iz = (leftSide.Iz * rightSide.Ix) + (leftSide.Jz * rightSide.Iy) + (leftSide.Kz * rightSide.Iz) + (leftSide.Tz * rightSide.Iw);
	Iw = (leftSide.Iw * rightSide.Ix) + (leftSide.Jw * rightSide.Iy) + (leftSide.Kw * rightSide.Iz) + (leftSide.Tw * rightSide.Iw);
	Jx = (leftSide.Ix * rightSide.Jx) + (leftSide.Jx * rightSide.Jy) + (leftSide.Kx * rightSide.Jz) + (leftSide.Tx * rightSide.Jw);
	Jy = (leftSide.Iy * rightSide.Jx) + (leftSide.Jy * rightSide.Jy) + (leftSide.Ky * rightSide.Jz) + (leftSide.Ty * rightSide.Jw);
	Jz = (leftSide.Iz * rightSide.Jx) + (leftSide.Jz * rightSide.Jy) + (leftSide.Kz * rightSide.Jz) + (leftSide.Tz * rightSide.Jw);
	Jw = (leftSide.Iw * rightSide.Jx) + (leftSide.Jw * rightSide.Jy) + (leftSide.Kw * rightSide.Jz) + (leftSide.Tw * rightSide.Jw);
	Kx = (leftSide.Ix * rightSide.Kx) + (leftSide.Jx * rightSide.Ky) + (leftSide.Kx * rightSide.Kz) + (leftSide.Tx * rightSide.Kw);
	Ky = (leftSide.Iy * rightSide.Kx) + (leftSide.Jy * rightSide.Ky) + (leftSide.Ky * rightSide.Kz) + (leftSide.Ty * rightSide.Kw);
	Kz = (leftSide.Iz * rightSide.Kx) + (leftSide.Jz * rightSide.Ky) + (leftSide.Kz * rightSide.Kz) + (leftSide.Tz * rightSide.Kw);
	Kw = (leftSide.Iw * rightSide.Kx) + (leftSide.Jw * rightSide.Ky) + (leftSide.Kw * rightSide.Kz) + (leftSide.Tw * rightSide.Kw);
	Tx = (leftSide.Ix * rightSide.Tx) + (leftSide.Jx * rightSide.Ty) + (leftSide.Kx * rightSide.Tz) + (leftSide.Tx * rightSide.Tw);
	Ty = (leftSide.Iy * rightSide.Tx) + (leftSide.Jy * rightSide.Ty) + (leftSide.Ky * rightSide.Tz) + (leftSide.Ty * rightSide.Tw);
	Tz = (leftSide.Iz * rightSide.Tx) + (leftSide.Jz * rightSide.Ty) + (leftSide.Kz * rightSide.Tz) + (leftSide.Tz * rightSide.Tw);
	Tw = (leftSide.Iw * rightSide.Tx) + (leftSide.Jw * rightSide.Ty) + (leftSide.Kw * rightSide.Tz) + (leftSide.Tw * rightSide.Tw);
}


void Matrix44::Scale2D( float scaleX, float scaleY ) {
	Append(MakeScale2D(scaleX, scaleY));
}


void Matrix44::ScaleUniform2D( float scale ) {
	Append(MakeScaleUniform2D(scale));
}


void Matrix44::SetIdentity() {
	Ix = 1.f;
	Iy = 0.f;
	Iz = 0.f;
	Iw = 0.f;
	Jx = 0.f;
	Jy = 1.f;
	Jz = 0.f;
	Jw = 0.f;
	Kx = 0.f;
	Ky = 0.f;
	Kz = 1.f;
	Kw = 0.f;
	Tx = 0.f;
	Ty = 0.f;
	Tz = 0.f;
	Tw = 1.f;
}


void Matrix44::RotateDegrees2D( float degreesToRotate ) {
	Append(MakeRotationDegrees2D(degreesToRotate));
}


void Matrix44::Translate2D( const Vector2& translation ) {
	Append(MakeTranslation2D(translation));
}


Vector2 Matrix44::TransformDisplacement2D( const Vector2& displacement ) {

	float transformedX = (displacement.x * Ix) + (displacement.y * Jx);
	float transformedY = (displacement.x * Iy) + (displacement.y * Jy);

	return Vector2(transformedX, transformedY);
}


Vector2 Matrix44::TransformPosition2D( const Vector2& point ) {

	float transformedX = (point.x * Ix) + (point.y * Jx) + Tx;
	float transformedY = (point.x * Iy) + (point.y * Jy) + Ty;

	return Vector2(transformedX, transformedY);
}

Vector3 Matrix44::TransformPosition(const Vector3& point) const {
	float transformedX = (point.x * Ix) + (point.y * Jx) + (point.z * Kx) + Tx;
	float transformedY = (point.x * Iy) + (point.y * Jy) + (point.z * Ky) + Ty;
	float transformedZ = (point.x * Iz) + (point.y * Jz) + (point.z * Kz) + Tz;

	return Vector3(transformedX, transformedY, transformedZ);
}

Vector4 Matrix44::Transform(const Vector4& point) const {
	float transformedX = (point.x * Ix) + (point.y * Jx) + (point.z * Kx) + (point.w * Tx);
	float transformedY = (point.x * Iy) + (point.y * Jy) + (point.z * Ky) + (point.w * Ty);
	float transformedZ = (point.x * Iz) + (point.y * Jz) + (point.z * Kz) + (point.w * Tz);
	float transformedW = (point.x * Iw) + (point.y * Jw) + (point.z * Kw) + (point.w * Tw);

	return Vector4(transformedX, transformedY, transformedZ, transformedW);
}


Vector3 Matrix44::TransformDirection(const Vector3& direction) const {
	float transformedX = (direction.x * Ix) + (direction.y * Jx) + (direction.z * Kx);
	float transformedY = (direction.x * Iy) + (direction.y * Jy) + (direction.z * Ky);
	float transformedZ = (direction.x * Iz) + (direction.y * Jz) + (direction.z * Kz);

	return Vector3(transformedX, transformedY, transformedZ);
}


Matrix44 Matrix44::MakeOrtho2D( const Vector2& mins, const Vector2& maxs ) {

	Matrix44 ortho;

	ortho.Ix = 2.f / (maxs.x - mins.x);
	ortho.Jy = 2.f / (maxs.y - mins.y);
	ortho.Kz = 1.f;
	ortho.Tx = - (maxs.x + mins.x) / (maxs.x - mins.x);
	ortho.Ty = - (maxs.y + mins.y) / (maxs.y - mins.y);
	ortho.Tz = -1.f;

	return ortho;
}


Matrix44 Matrix44::MakeOrthographic(float left, float right, float top, float bottom, float far, float near) {
	Matrix44 ortho;
	ortho.Ix = 2.f / (right - left);
	ortho.Jy = 2.f / (top - bottom);
	ortho.Kz = 2.f / (far - near);
	ortho.Tx = -(right + left) / (right - left);
	ortho.Ty = -(top + bottom) / (top - bottom);
	ortho.Tz = -(far + near) / (far - near);
	ortho.Tw = 1.f;
	return ortho;
}


Matrix44 Matrix44::MakeRotationDegrees2D( float degreesToRotate ) {

	Matrix44 rotation;

	rotation.Ix = CosDegrees(degreesToRotate);
	rotation.Iy = SinDegrees(degreesToRotate);
	rotation.Jx = -SinDegrees(degreesToRotate);
	rotation.Jy = CosDegrees(degreesToRotate);

	return rotation;
}


Matrix44 Matrix44::MakeScale2D( float scaleX, float scaleY ) {

	Matrix44 scale;

	scale.Ix = scaleX;
	scale.Jy = scaleY;

	return scale;
}


Matrix44 Matrix44::MakeScaleUniform2D( float scale ) {

	Matrix44 scaleMatrix;

	scaleMatrix.Ix = scale;
	scaleMatrix.Jy = scale;

	return scaleMatrix;
}


Matrix44 Matrix44::MakeTranslation2D( const Vector2& translation ) {

	Matrix44 translate;

	translate.Tx = translation.x;
	translate.Ty = translation.y;

	return translate;
}


Matrix44 Matrix44::MakeProjection( float fov, float aspect, float zNear, float zFar ) {

	/*float halfHeight = zNear * TanDegrees(fov / 2.f);
	float halfWidth = halfHeight * aspect;
	float depth = zFar - zNear;*/

	float d = 1.f / (TanDegrees(fov / 2.f));
	float depth = zFar - zNear;


	Matrix44 projection;

	projection.Ix = d / aspect;
	projection.Jy = d;
	projection.Kz = (zFar + zNear) / depth;
	projection.Kw = 1.f;
	projection.Tw = 0.f;
	projection.Tz = -(2.f * zFar * zNear) / depth;

	return projection;

}


Matrix44 Matrix44::MakeTranslation( const Vector3& translation ) {
	Matrix44 translate;
	translate.Tx = translation.x;
	translate.Ty = translation.y;
	translate.Tz = translation.z;
	return translate;
}


Matrix44 Matrix44::GetTranspose() const {
	Matrix44 t;
	t.Ix = Ix;
	t.Iy = Jx;
	t.Iz = Kx;
	t.Iw = Tw;
	t.Jx = Iy;
	t.Jy = Jy;
	t.Jz = Ky;
	t.Jw = Ty;
	t.Kx = Iz;	
	t.Ky = Jz;
	t.Kz = Kz;
	t.Kw = Tw;
	t.Tx = Iw;
	t.Ty = Jw;
	t.Tz = Kw;
	t.Tw = Tw;
	return t;
}


Matrix44 Matrix44::GetInverseFast() const {
	Matrix44 inv;
	Vector3 translation( Tx, Ty, Tz );

	inv.Ix = Ix;
	inv.Iy = Jx;
	inv.Iz = Kx;
	//inv.Iw = -Tx;
	inv.Jx = Iy;
	inv.Jy = Jy;
	inv.Jz = Ky;
	//inv.Jw = -Ty;
	inv.Kx = Iz;
	inv.Ky = Jz;
	inv.Kz = Kz;
	//inv.Kw = -Tz;
	//inv.Tx = Tx;
	//inv.Ty = Ty;
	//inv.Tz = Tz;

	Vector3 translationInv = inv.TransformPosition(translation) * -1.f;
	inv.Tx = translationInv.x;
	inv.Ty = translationInv.y;
	inv.Tz = translationInv.z;

	return inv;

}


Matrix44 Matrix44::MakeRotationDegrees( const Vector3& rotation ) {

	Matrix44 xRot;
	xRot.Jy = CosDegrees(rotation.x);
	xRot.Jz = -SinDegrees(rotation.x);
	xRot.Ky = SinDegrees(rotation.x);
	xRot.Kz = CosDegrees(rotation.x);

	Matrix44 yRot;
	yRot.Ix = CosDegrees(rotation.y);
	yRot.Iz = -SinDegrees(rotation.y);
	yRot.Kx = SinDegrees(rotation.y);
	yRot.Kz = CosDegrees(rotation.y);

	Matrix44 zRot;
	zRot.Ix = CosDegrees(rotation.z);
	zRot.Iy = SinDegrees(rotation.z);
	zRot.Jx = -SinDegrees(rotation.z);
	zRot.Jy = CosDegrees(rotation.z);

	yRot.Append(xRot);
	yRot.Append(zRot);

	return yRot;
}


Vector3 Matrix44::GetRight() const {
	return Vector3(Ix, Iy, Iz).GetNormalized();
}

Vector3 Matrix44::GetUp() const {
	return Vector3(Jx, Jy, Jz).GetNormalized();
}

Vector3 Matrix44::GetForward() const {
	return Vector3(Kx, Ky, Kz).GetNormalized();
}


Matrix44 Matrix44::MakeScale( const Vector3& scale ) {
	Matrix44 scaleMat;
	scaleMat.Ix = scale.x;
	scaleMat.Jy = scale.y;
	scaleMat.Kz = scale.z;
	return scaleMat;
}


//------------------------------------------------------------------------
// Lifted from GLU
void Matrix44::Invert()
{
	double inv[16];
	double det;
	double m[16];
	unsigned int i;
	m[0] = Ix;
	m[1] = Iy;
	m[2] = Iz;
	m[3] = Iw;
	m[4] = Jx;
	m[5] = Jy;
	m[6] = Jz;
	m[7] = Jw;
	m[8] = Kx;
	m[9] = Ky;
	m[10] = Kz;
	m[11] = Kw;
	m[12] = Tx;
	m[13] = Ty;
	m[14] = Tz;
	m[15] = Tw;

	inv[0] = m[5]  * m[10] * m[15] - 
		m[5]  * m[11] * m[14] - 
		m[9]  * m[6]  * m[15] + 
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] - 
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] + 
		m[4]  * m[11] * m[14] + 
		m[8]  * m[6]  * m[15] - 
		m[8]  * m[7]  * m[14] - 
		m[12] * m[6]  * m[11] + 
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] - 
		m[4]  * m[11] * m[13] - 
		m[8]  * m[5] * m[15] + 
		m[8]  * m[7] * m[13] + 
		m[12] * m[5] * m[11] - 
		m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] + 
		m[4]  * m[10] * m[13] +
		m[8]  * m[5] * m[14] - 
		m[8]  * m[6] * m[13] - 
		m[12] * m[5] * m[10] + 
		m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] + 
		m[1]  * m[11] * m[14] + 
		m[9]  * m[2] * m[15] - 
		m[9]  * m[3] * m[14] - 
		m[13] * m[2] * m[11] + 
		m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] - 
		m[0]  * m[11] * m[14] - 
		m[8]  * m[2] * m[15] + 
		m[8]  * m[3] * m[14] + 
		m[12] * m[2] * m[11] - 
		m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] + 
		m[0]  * m[11] * m[13] + 
		m[8]  * m[1] * m[15] - 
		m[8]  * m[3] * m[13] - 
		m[12] * m[1] * m[11] + 
		m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] - 
		m[0]  * m[10] * m[13] - 
		m[8]  * m[1] * m[14] + 
		m[8]  * m[2] * m[13] + 
		m[12] * m[1] * m[10] - 
		m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] - 
		m[1]  * m[7] * m[14] - 
		m[5]  * m[2] * m[15] + 
		m[5]  * m[3] * m[14] + 
		m[13] * m[2] * m[7] - 
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] + 
		m[0]  * m[7] * m[14] + 
		m[4]  * m[2] * m[15] - 
		m[4]  * m[3] * m[14] - 
		m[12] * m[2] * m[7] + 
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] - 
		m[0]  * m[7] * m[13] - 
		m[4]  * m[1] * m[15] + 
		m[4]  * m[3] * m[13] + 
		m[12] * m[1] * m[7] - 
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] + 
		m[0]  * m[6] * m[13] + 
		m[4]  * m[1] * m[14] - 
		m[4]  * m[2] * m[13] - 
		m[12] * m[1] * m[6] + 
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] + 
		m[1] * m[7] * m[10] + 
		m[5] * m[2] * m[11] - 
		m[5] * m[3] * m[10] - 
		m[9] * m[2] * m[7] + 
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] - 
		m[0] * m[7] * m[10] - 
		m[4] * m[2] * m[11] + 
		m[4] * m[3] * m[10] + 
		m[8] * m[2] * m[7] - 
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] + 
		m[0] * m[7] * m[9] + 
		m[4] * m[1] * m[11] - 
		m[4] * m[3] * m[9] - 
		m[8] * m[1] * m[7] + 
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] - 
		m[0] * m[6] * m[9] - 
		m[4] * m[1] * m[10] + 
		m[4] * m[2] * m[9] + 
		m[8] * m[1] * m[6] - 
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	float data[16];
	for (i = 0; i < 16; i++) {
		data[i] = (float)(inv[i] * det);
	}
	SetValues(data);
}


Matrix44 Matrix44::GetInverse() const
{
	double inv[16];
	double det;
	double m[16];
	unsigned int i;
	m[0] = Ix;
	m[1] = Iy;
	m[2] = Iz;
	m[3] = Iw;
	m[4] = Jx;
	m[5] = Jy;
	m[6] = Jz;
	m[7] = Jw;
	m[8] = Kx;
	m[9] = Ky;
	m[10] = Kz;
	m[11] = Kw;
	m[12] = Tx;
	m[13] = Ty;
	m[14] = Tz;
	m[15] = Tw;

	inv[0] = m[5]  * m[10] * m[15] - 
		m[5]  * m[11] * m[14] - 
		m[9]  * m[6]  * m[15] + 
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] - 
		m[13] * m[7]  * m[10];

	inv[4] = -m[4]  * m[10] * m[15] + 
		m[4]  * m[11] * m[14] + 
		m[8]  * m[6]  * m[15] - 
		m[8]  * m[7]  * m[14] - 
		m[12] * m[6]  * m[11] + 
		m[12] * m[7]  * m[10];

	inv[8] = m[4]  * m[9] * m[15] - 
		m[4]  * m[11] * m[13] - 
		m[8]  * m[5] * m[15] + 
		m[8]  * m[7] * m[13] + 
		m[12] * m[5] * m[11] - 
		m[12] * m[7] * m[9];

	inv[12] = -m[4]  * m[9] * m[14] + 
		m[4]  * m[10] * m[13] +
		m[8]  * m[5] * m[14] - 
		m[8]  * m[6] * m[13] - 
		m[12] * m[5] * m[10] + 
		m[12] * m[6] * m[9];

	inv[1] = -m[1]  * m[10] * m[15] + 
		m[1]  * m[11] * m[14] + 
		m[9]  * m[2] * m[15] - 
		m[9]  * m[3] * m[14] - 
		m[13] * m[2] * m[11] + 
		m[13] * m[3] * m[10];

	inv[5] = m[0]  * m[10] * m[15] - 
		m[0]  * m[11] * m[14] - 
		m[8]  * m[2] * m[15] + 
		m[8]  * m[3] * m[14] + 
		m[12] * m[2] * m[11] - 
		m[12] * m[3] * m[10];

	inv[9] = -m[0]  * m[9] * m[15] + 
		m[0]  * m[11] * m[13] + 
		m[8]  * m[1] * m[15] - 
		m[8]  * m[3] * m[13] - 
		m[12] * m[1] * m[11] + 
		m[12] * m[3] * m[9];

	inv[13] = m[0]  * m[9] * m[14] - 
		m[0]  * m[10] * m[13] - 
		m[8]  * m[1] * m[14] + 
		m[8]  * m[2] * m[13] + 
		m[12] * m[1] * m[10] - 
		m[12] * m[2] * m[9];

	inv[2] = m[1]  * m[6] * m[15] - 
		m[1]  * m[7] * m[14] - 
		m[5]  * m[2] * m[15] + 
		m[5]  * m[3] * m[14] + 
		m[13] * m[2] * m[7] - 
		m[13] * m[3] * m[6];

	inv[6] = -m[0]  * m[6] * m[15] + 
		m[0]  * m[7] * m[14] + 
		m[4]  * m[2] * m[15] - 
		m[4]  * m[3] * m[14] - 
		m[12] * m[2] * m[7] + 
		m[12] * m[3] * m[6];

	inv[10] = m[0]  * m[5] * m[15] - 
		m[0]  * m[7] * m[13] - 
		m[4]  * m[1] * m[15] + 
		m[4]  * m[3] * m[13] + 
		m[12] * m[1] * m[7] - 
		m[12] * m[3] * m[5];

	inv[14] = -m[0]  * m[5] * m[14] + 
		m[0]  * m[6] * m[13] + 
		m[4]  * m[1] * m[14] - 
		m[4]  * m[2] * m[13] - 
		m[12] * m[1] * m[6] + 
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] + 
		m[1] * m[7] * m[10] + 
		m[5] * m[2] * m[11] - 
		m[5] * m[3] * m[10] - 
		m[9] * m[2] * m[7] + 
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] - 
		m[0] * m[7] * m[10] - 
		m[4] * m[2] * m[11] + 
		m[4] * m[3] * m[10] + 
		m[8] * m[2] * m[7] - 
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] + 
		m[0] * m[7] * m[9] + 
		m[4] * m[1] * m[11] - 
		m[4] * m[3] * m[9] - 
		m[8] * m[1] * m[7] + 
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] - 
		m[0] * m[6] * m[9] - 
		m[4] * m[1] * m[10] + 
		m[4] * m[2] * m[9] + 
		m[8] * m[1] * m[6] - 
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
	det = 1.0 / det;

	float data[16];
	for (i = 0; i < 16; i++) {
		data[i] = (float)(inv[i] * det);
	}
	return Matrix44(data);
}


Vector3 Matrix44::GetTranslation() const {
	return Vector3(Tx, Ty, Tz);
}

Vector3 Matrix44::GetRotation() const {
	float xEuler;
	float yEuler;
	float zEuler;
	
	float xSin = -Ky;
	xSin = ClampFloatNegativeOneToOne(xSin);
	xEuler = -AsinDegrees(xSin);

	float xCos = CosDegrees(xEuler);
	if (!(abs(xCos) < 0.0000001f)) {
		yEuler = Atan2Degrees( Kx, Kz );
		zEuler = Atan2Degrees( Iy, Jy );
	}
	else {
		zEuler = 0.f;
		yEuler = Atan2Degrees( -Iz, Ix );
	}

	return Vector3( xEuler, yEuler, zEuler );
}


float Matrix44::GetTrace3() const {
	return Ix + Jy + Kz;
}


Matrix44 InterpolateRotation( const Matrix44& start, const Matrix44& end, float fractionTowards ) {

	Vector3 translation = start.GetTranslation();

	Vector3 startRight = start.GetRight();
	Vector3 endRight = end.GetRight();
	Vector3 startUp = start.GetUp();
	Vector3 endUp = end.GetUp();
	Vector3 startForward = start.GetForward();
	Vector3 endForward = end.GetForward();

	Vector3 right = SlerpUnitVectors(startRight, endRight, fractionTowards);
	Vector3 up = SlerpUnitVectors(startUp, endUp, fractionTowards);
	Vector3 forward = SlerpUnitVectors(startForward, endForward, fractionTowards);

	return Matrix44(right, up, forward, translation);

}


Matrix44 LerpMatrix( const Matrix44& start, const Matrix44& end, float fractionToward ) {
	Vector3 rightStart = start.GetRight();
	Vector3 rightEnd = end.GetRight();
	Vector3 forwardStart = start.GetForward();
	Vector3 forwardEnd = end.GetForward();
	Vector3 upStart = start.GetUp();
	Vector3 upEnd = end.GetUp();
	Vector3 translateStart = start.GetTranslation();
	Vector3 translateEnd = end.GetTranslation();

	Vector3 right = Slerp( rightStart, rightEnd, fractionToward );
	Vector3 forward = Slerp( forwardStart, forwardEnd, fractionToward );
	Vector3 up = Slerp( upStart, upEnd, fractionToward );
	Vector3 translate = Interpolate( translateStart, translateEnd, fractionToward );

	return Matrix44( right, up, forward, translate );
} 