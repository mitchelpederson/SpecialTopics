#include "Engine/Math/Vector4.hpp"


Vector4::Vector4(float X, float Y, float Z, float W)
	: x(X), y(Y), z(Z), w(W)  
{}

Vector4::Vector4(const Vector3& xyz, float W) 
	: x(xyz.x), y(xyz.y), z(xyz.z), w(W)
{}

Vector4::Vector4() 
	: x(0.f), y(0.f), z(0.f), w(0.f)
{}

Vector3 Vector4::xyz() const {
	return Vector3(x, y, z);
}

Vector4 Vector4::CrossProduct( const Vector4& a, const Vector4& b ) {
	return Vector4(Vector3::CrossProduct( a.xyz(), b.xyz() ), 0);
}


const Vector4 Vector4::operator*( float c ) const {
	return Vector4(x*c, y*c, z*c, w*c);
}

const Vector4 Vector4::operator-(const Vector4& b) const {
	return Vector4(x - b.x, y - b.y, z - b.z, w - b.w);
}

const Vector4 Vector4::operator+(const Vector4& b) const {
	return Vector4(x + b.x, y + b.y, z + b.z, w + b.w);
}