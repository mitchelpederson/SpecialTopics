#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Vector3.hpp"
#include "Engine\Math\AABB2.hpp"
#include "Engine\Math\Disc2.hpp"

	// Random number generation functions
float	GetRandomFloatZeroToOne();
float	GetRandomFloatInRange( float min, float max );
int		GetRandomIntInRange( int min, int max );
int		GetRandomIntLessThan( int max );
int		GetRandomPositiveOrNegative();
bool	CheckRandomChance( float chanceForSuccess );
Vector3 GetRandomUnitVector();
Vector3 GetRandomDirectionInCone( float angle );

	// Trig
float	ConvertDegreesToRadians( float degrees );
float	ConvertRadiansToDegrees( float radians );
float	CosDegrees( float degrees );
float	SinDegrees( float degrees );
float	Atan2Degrees( float opposite, float adjacent );
float	AsinDegrees( float val );
float	AcosDegrees( float val );
float	TanDegrees( float degrees );
float	GetDistance( const Vector2& a, const Vector2& b );
float	GetDistanceSquared( const Vector2& a, const Vector2& b );
Vector3	PolarToCartesian3D( float radius, float theta, float phi );


	// Round, Clamp and Range Mapping
bool	IsFloatNearlyZero( float value );
bool	AreFloatsNearlyEqual( float a, float b );
int		RoundToNearestInt( float inValue );
int		ClampInt( int value, int min, int max );
float	ClampFloat( float value, float min, float max );
float	ClampFloatZeroToOne( float value );
float	ClampFloatNegativeOneToOne( float value );
float	RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd );
float	GetFractionInRange( float inValue, float rangeStart, float rangeEnd );
float	Interpolate( float start, float end, float fractionTowardEnd );
int		Interpolate( int start, int end, float fractionTowardEnd );
unsigned char Interpolate( unsigned char start, unsigned char end, float fractionToward );



	// Bitwise
bool	IsBitSet( unsigned short flags, unsigned short mask );
bool	AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck );
bool	AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck );
void	SetBits( unsigned char& bitFlags8, unsigned char flagsToSet );
void	SetBits( unsigned int& bitFlags32, unsigned int flagsToSet );
void	ClearBits( unsigned char& bitFlags8, unsigned char flagToClear );
void	ClearBits( unsigned int& bitFlags32, unsigned int flagToClear );



	// Angle & Vector
float	GetAngularDisplacement(float start, float end);
float	TurnToward( float startDegrees, float goalDegrees, float maxDelta );
float	DotProduct( const Vector2& a, const Vector2& b );
float	DotProduct( const Vector3& a, const Vector3& b );
float	GetNormalizedAngle( float angleDegrees );
bool	DoDiscAndAABBOverlap (const Disc2& disc, const AABB2& aabb);


	// Easing functions
float	SmoothStart2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease in”)
float	SmoothStart3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease in”)
float	SmoothStart4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease in”)
float	SmoothStop2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease out”)
float	SmoothStop3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease out”)
float	SmoothStop4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease out”)
float	SmoothStep3( float t ); // 3rd-degree smooth start/stop (a.k.a. “smoothstep”)


// Cyclic
template <typename T>
bool CyclicLess( const T& a, const T& b ) {
	T diff = b - a;
	T check =  T(0);
	check = ~check;
	check = check >> 1;
	if ( diff < check ) {
		return true;
	} else {
		return false;
	}
}


	// Max/Min
int		Max(float a, float b, float c, float d);

template <typename T>
T Min( T a, T b ) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}


template <typename T>
T Max( T a, T b ) {
	if (a > b) {
		return a;
	} else {
		return b;
	}
}

bool	Quadratic( float& out_one, float& out_two, float a, float b, float c );


Vector3 Average(const Vector3& a, const Vector3& b);