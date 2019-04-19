#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>
#include <string>

const Vector2 Vector2::NORTH = Vector2(	0.f,  1.f);
const Vector2 Vector2::SOUTH = Vector2(	0.f, -1.f);
const Vector2 Vector2::EAST  = Vector2(	1.f,  0.f);
const Vector2 Vector2::WEST  = Vector2(-1.f,  0.f);
const Vector2 Vector2::ZERO  = Vector2( 0.f,  0.f);
const Vector2 Vector2::ZERO_TO_ONE = Vector2(0.f, 1.f);


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( const Vector2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//----------------------------------------------------------------------------------------------------------------
Vector2::Vector2( const IntVector2& copyFrom ) 
	: x( (float) copyFrom.x )
	, y( (float) copyFrom.y )
{
}


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
float Vector2::GetLength() const {
	return sqrtf((x * x) + (y * y));
}


//-----------------------------------------------------------------------------------------------
float Vector2::GetLengthSquared() const {
	return (x * x) + (y * y);
}


//-----------------------------------------------------------------------------------------------
float Vector2::NormalizeAndGetLength() {

	float length = GetLength();
	x = x / length;
	y = y / length;

	return length;
}


//-----------------------------------------------------------------------------------------------
Vector2 Vector2::GetNormalized() const {

	float length = GetLength();
	return Vector2( x / length, y / length );
}


//-----------------------------------------------------------------------------------------------
float Vector2::GetOrientationDegrees() const {
	return ConvertRadiansToDegrees( atan2f( y, x ) );
}


//-----------------------------------------------------------------------------------------------
Vector2 Vector2::MakeDirectionAtDegrees( float degrees ) {

	return Vector2(CosDegrees(degrees), SinDegrees(degrees));

}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator+( const Vector2& vecToAdd ) const
{
	return Vector2( x + vecToAdd.x, y + vecToAdd.y ); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator-( const Vector2& vecToSubtract ) const
{
	return Vector2( x - vecToSubtract.x, y - vecToSubtract.y ); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator*( float uniformScale ) const
{
	return Vector2( x * uniformScale, y * uniformScale ); 
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator/( float inverseScale ) const
{
	return Vector2( x / inverseScale, y / inverseScale ); 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator+=( const Vector2& vecToAdd )
{
	x += vecToAdd.x; 
	y += vecToAdd.y; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator-=( const Vector2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator*=( const float uniformScale )
{
	x *= uniformScale; 
	y *= uniformScale; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor; 
	y /= uniformDivisor; 
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator=( const Vector2& copyFrom )
{
	x = copyFrom.x; 
	y = copyFrom.y; 
}


//-----------------------------------------------------------------------------------------------
const Vector2 operator*( float uniformScale, const Vector2& vecToScale )
{
	return Vector2( uniformScale * vecToScale.x, uniformScale * vecToScale.y ); 
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator==( const Vector2& compare ) const
{
	if (x == compare.x && y == compare.y) {
		return true;
	}
	else {
		return false;
	}
 
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator!=( const Vector2& compare ) const
{
	if (x == compare.x && y == compare.y) {
		return false;
	}
	else {
		return true;
	} 
}


Vector2	Reflect( const Vector2& vectorToReflect, const Vector2& normal ) {

	Vector2 vectorToReflectProjection = GetProjectedVector(vectorToReflect, normal);
	return vectorToReflect - (vectorToReflectProjection * 2.f);

}


const Vector2 GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto ) {
	
	Vector2 projectOntoNormalized = projectOnto.GetNormalized();

	float projectionMagnitude = DotProduct(vectorToProject, projectOntoNormalized);

	return projectOntoNormalized * projectionMagnitude;

}


const Vector2 GetTransformedIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ ) {
	
	float magnitudeOnI = DotProduct(originalVector, newBasisI);
	float magnitudeOnJ = DotProduct(originalVector, newBasisJ);

	return Vector2(magnitudeOnI, magnitudeOnJ);

}


const Vector2 GetTransformedOutOfBasis( const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ ) {
	
	Vector2 cartesianX(1.f, 0.f);
	Vector2 cartesianY(0.f, 1.f);

	float magnitudeIOnX = DotProduct(oldBasisI, cartesianX);
	float magnitudeIOnY = DotProduct(oldBasisJ, cartesianX);
	float magnitudeJOnX = DotProduct(oldBasisI, cartesianY);
	float magnitudeJOnY = DotProduct(oldBasisJ, cartesianY);

	Vector2 basisIOnCartesian(magnitudeIOnX, magnitudeIOnY);
	Vector2 basisJOnCartesian(magnitudeJOnX, magnitudeJOnY);

	float magnitudeVectorInCartesianX = DotProduct(vectorInBasis, basisIOnCartesian);
	float magnitudeVectorInCartesianY = DotProduct(vectorInBasis, basisJOnCartesian);
	
	return Vector2( magnitudeVectorInCartesianX, magnitudeVectorInCartesianY );

}


void DecomposeVectorIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ, Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ ) {

	float magnitudeOnI = DotProduct(originalVector, newBasisI);
	float magnitudeOnJ = DotProduct(originalVector, newBasisJ);

	out_vectorAlongI = magnitudeOnI * newBasisI;
	out_vectorAlongJ = magnitudeOnJ * newBasisJ;

}


Vector2 Interpolate ( const Vector2& start, const Vector2& end, float fractionToward ) {

	return Vector2( Interpolate(start.x, end.x, fractionToward), Interpolate(start.y, end.y, fractionToward));

}


void Vector2::SetFromText( const char* text ) {

	int commaIndex = 0;

	while (text[commaIndex] != ',') {
		commaIndex++;
	}

	std::string textCopy(text);

	x = (float) atof(textCopy.substr(0, commaIndex).c_str());
	y = (float) atof(textCopy.substr(commaIndex + 1, textCopy.length() - 1).c_str());

}
