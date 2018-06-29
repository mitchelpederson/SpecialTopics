
#include "MathUtils.hpp"
#include <stdlib.h>
#include <math.h>
#include <iostream>


float GetRandomFloatZeroToOne() {
	return (float) rand() / (float) RAND_MAX;
}


// Inclusive
float GetRandomFloatInRange(float min, float max) {

	float range = max - min;

	return GetRandomFloatZeroToOne() * range + min;
}


// Inclusive
int GetRandomIntInRange(int min, int max) {

	int range = max - min + 1;

	return rand() % range + min;

}


int GetRandomIntLessThan(int max) {
	return rand() % max;
}


int GetRandomPositiveOrNegative ( ) {
	if (rand() % 2 == 0) {
		return -1;
	} 
	else {
		return 1;
	}
}


bool CheckRandomChance( float chanceForSuccess ) {

	if (GetRandomFloatZeroToOne() < chanceForSuccess) {
		return false;
	}
	else {
		return true;
	}

}


float ConvertDegreesToRadians(float degrees) {
	return degrees * (3.1415926535897932f / 180.f);
}


float ConvertRadiansToDegrees(float radians) {
	return radians * (180.f / 3.1415926535897932f);
}


float CosDegrees(float degrees) {
	return cosf(ConvertDegreesToRadians(degrees));
}


float SinDegrees(float degrees) {
	return sinf(ConvertDegreesToRadians(degrees));
}

float TanDegrees(float degrees) {
	return tanf(ConvertDegreesToRadians(degrees));
}

float Atan2Degrees(float opposite, float adjacent) {
	return ConvertRadiansToDegrees( (float) atan2( opposite, adjacent ) ); 
}

float AsinDegrees( float val ) {
	return ConvertRadiansToDegrees( (float) asin( val ) );
}

float AcosDegrees( float val ) {
	return ConvertRadiansToDegrees( (float) acos( val ) );
}


float GetDistance(const Vector2& a, const Vector2& b) {
	float yDist = a.y - b.y;
	float xDist = a.x - b.x;

	return sqrtf( (yDist * yDist) + (xDist * xDist) );
}


float GetDistanceSquared(const Vector2& a, const Vector2& b) {
	float yDist = a.y - b.y;
	float xDist = a.x - b.x;

	return (yDist * yDist) + (xDist * xDist);
}


int RoundToNearestInt( float inValue ) {

	int result = 0;

	if (inValue > 0.0f) {
		result = (int) roundf(inValue);
	}
	else {

		result = (int) roundf(inValue);

		if ( (fmodf(inValue * -10.f, 10.f)) == 5.f ) {
			result += 1;
		}

	}

	return result;

}


int ClampInt( int value, int min, int max ) {
	if (value < min) {
		return min;
	}
	else if (value > max) {
		return max;
	}
	else {
		return value;
	}
}


float ClampFloat( float value, float min, float max ) {
	if (value < min) {
		return min;
	}
	else if (value > max) {
		return max;
	}
	else {
		return value;
	}
} 


float ClampFloatZeroToOne( float value ) {
	if (value < 0.f) {
		return 0.f;
	}

	else if (value > 1.f) {
		return 1.f;
	}

	else {
		return value;
	}
}


float ClampFloatNegativeOneToOne( float value ) {
	if (value < -1.f) {
		return -1.f;
	}

	else if (value > 1.f) {
		return 1.f;
	}

	else {
		return value;
	}
}


float RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd ) {
	float inRange = inEnd - inStart;

	if (inRange == 0) {
		return outStart;
	}

	float outRange = outEnd - outStart;
	float inRelative = inValue - inStart;
	float fractionInto = inRelative / inRange;
	float outRelative = fractionInto * outRange;
	
	return outRelative + outStart;
}


float GetFractionInRange( float inValue, float rangeStart, float rangeEnd ) {
	float range = rangeEnd - rangeStart;
	return fabsf((inValue - rangeStart) / range);
}


float Interpolate( float start, float end, float fractionTowardEnd ) {
	float range = end - start;
	return (range * fractionTowardEnd) + start;
}


int Interpolate( int start, int end, float fractionTowardEnd ) {

	int range = end - start;
	float interpolatedDistance = (float) range * fractionTowardEnd;
	return RoundToNearestInt(interpolatedDistance) + start;
}

unsigned char Interpolate( unsigned char start, unsigned char end, float fractionToward ) {
	return (unsigned char) Interpolate ((int) start, (int) end, fractionToward);
}



bool IsBitSet( unsigned short flags, unsigned short mask ) {
	return ( flags & mask ) == mask;
}


float GetAngularDisplacement( float start, float end ) {

	float angularDisplacement = end - start;

	angularDisplacement = GetNormalizedAngle(angularDisplacement);

	return angularDisplacement;

}


float TurnToward( float startDegrees, float goalDegrees, float maxDelta ) {
	float distanceToGoal = GetAngularDisplacement(startDegrees, goalDegrees);

	if (distanceToGoal > maxDelta) {
		return startDegrees + maxDelta;
	}

	else if (distanceToGoal < -maxDelta) {
		return startDegrees - maxDelta;
	}

	else {
		return goalDegrees;
	}
}


float DotProduct( const Vector2& a, const Vector2& b ) {

	
	return (a.x * b.x) + (a.y * b.y);
}


float DotProduct( const Vector3& a, const Vector3& b ) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}


float GetNormalizedAngle( float angleDegrees ) {
	
	while (angleDegrees > 180.f) {
		angleDegrees -= 360.f;
	}

	while (angleDegrees < -180.f) {
		angleDegrees += 360.f;
	}

	return angleDegrees;
}



bool DoDiscAndAABBOverlap( const Disc2& disc, const AABB2& aabb) {

	Vector2 aabbCenter = aabb.GetCenter();
	//float displacementAngle = discToAABBDisplacement.GetOrientationDegrees();

	// top right
	if ( disc.IsPointInside(aabb.maxs.x, aabb.maxs.y) ) {
		return true;
	}

	// bottom right
	if ( disc.IsPointInside(aabb.maxs.x, aabb.mins.y) ) {
		return true;
	}

	// bottom left
	if ( disc.IsPointInside(aabb.mins.x, aabb.mins.y) ) {
		return true;
	}

	// top left
	if ( disc.IsPointInside(aabb.mins.x, aabb.maxs.y) ) {
		return true;
	}

	if ( aabb.IsPointInside(disc.center) ) {
		return true;
	}

		// Check above
	if ( aabb.IsPointInside(disc.center.x, disc.center.y - disc.radius) ) {
		return true;
	}

		// Check below
	if ( aabb.IsPointInside(disc.center.x, disc.center.y + disc.radius) ) {
		return true; 
	}

		// Check left
	if ( aabb.IsPointInside(disc.center.x + disc.radius, disc.center.y) ) {
		return true;
	}
		
		// Check right
	if (  aabb.IsPointInside(disc.center.x - disc.radius, disc.center.y) ) {
		return true;
	}

	return false;
}



bool AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck ) {
	return ( bitFlags8 & flagsToCheck ) == flagsToCheck;
}


bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck ) {
	return ( bitFlags32 & flagsToCheck ) == flagsToCheck;

}


void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet ) {
	bitFlags8 = bitFlags8 | flagsToSet;
}


void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet ) {
	bitFlags32 = bitFlags32 | flagsToSet;

}


void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear ) {
	bitFlags8 = bitFlags8 & ~flagToClear;
}


void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear ) {
	bitFlags32 = bitFlags32 & ~flagToClear;
}



float	SmoothStart2( float t ) {
	return t * t;
}
float	SmoothStart3( float t ) {
	return t * t * t;
}
float	SmoothStart4( float t ) {
	return t * t * t * t;
}
float	SmoothStop2( float t ) {
	return 1 - ((1-t) * (1-t));
}
float	SmoothStop3( float t ) {
	return 1 - ((1-t) * (1-t) * (1-t));
}
float	SmoothStop4( float t ) {
	return 1 - ((1-t) * (1-t) * (1-t) * (1-t));
}
float	SmoothStep3( float t ) {
		return ((1-t) * SmoothStart3(t)) + (t * SmoothStop3(t));
}


int Max(float a, float b, float c, float d) {

	if (a >= b && a >= c && a >= d) {
		return 0;
	}

	else if ( b >= a && b >= c && b >= d) {
		return 1;
	}

	else if (c >= a && c >= b && c >= d) {
		return 2;
	}

	else {
		return 3;
	}
}


bool Quadratic( float& out_lesser, float& out_greater, float a, float b, float c ) {

	float insideSqrt = (b * b) - (4.f * a * c);
	if (insideSqrt < 0.f) {
		return false;
	}

	float numeratorPlus = -b + insideSqrt;
	float numeratorMinus = -b - insideSqrt;
	float rootPlus = numeratorPlus / (2.f * a);
	float rootMinus = numeratorMinus / (2.f * a);

	if (rootPlus > rootMinus) {
		out_lesser = rootMinus;
		out_greater = rootPlus;
	} else {
		out_lesser = rootPlus;
		out_greater = rootMinus;
	}

	return true;
}


Vector3 PolarToCartesian3D( float radius, float theta, float phi ) {
	float x = radius * CosDegrees(phi) * CosDegrees(theta);
	float y = radius * SinDegrees(phi);
	float z = radius * CosDegrees(phi) * SinDegrees(theta);
	return Vector3(x, y, z);
}


Vector3 GetRandomUnitVector() {
	float theta = GetRandomFloatInRange(0.f, 360.f);
	float phi = GetRandomFloatInRange(-90.f, 90.f);
	return PolarToCartesian3D(1.f, theta, phi);
}


Vector3 GetRandomDirectionInCone( float angle ) {

		float theta = GetRandomFloatInRange(0.f, angle);
		float phi = GetRandomFloatInRange(-angle * 0.5f, angle * 0.5f);

		return PolarToCartesian3D(1.f, theta, phi);

}


Vector3 Average(const Vector3& a, const Vector3& b) {
	return (a + b) * 0.5f;
}


float Min(float a, float b) {
	if (a < b) {
		return a;
	}
	else {
		return b;
	}
}