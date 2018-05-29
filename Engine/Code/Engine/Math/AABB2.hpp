#pragma once
#include "Engine\Math\Vector2.hpp"
#include "Engine\Math\Disc2.hpp"

class AABB2 {

public:
	Vector2 mins;
	Vector2 maxs;

			 AABB2();
			 AABB2( const AABB2& copy );
	explicit AABB2( float minX, float minY, float maxX, float maxY );
	explicit AABB2( const Vector2& mins, const Vector2& maxs );
	explicit AABB2( const Vector2& center, float radiusX, float radiusY );
			 ~AABB2();

	static const AABB2 ZERO_TO_ONE;

	void StretchToIncludePoint( float x, float y );
	void StretchToIncludePoint( const Vector2& point );
	void AddPaddingToSides( float xPaddingRadius, float yPaddingRadius );
	void Translate( const Vector2& translation );
	void Translate( float translationX, float translationY );
	void SetFromText(const char* text);

	bool IsPointInside( float x, float y) const;
	bool IsPointInside( const Vector2& point ) const;
	Vector2 GetDimensions() const;
	Vector2 GetCenter() const;
	Vector2 GetNearestPointOnBounds( const Vector2& outsidePoint );


	void operator+=( const Vector2& translation );
	void operator-=( const Vector2& antiTranslation );
	AABB2 operator+( const Vector2& translation ) const;
	AABB2 operator-( const Vector2& antiTranslation ) const;
};


// Utilities
bool DoAABBsOverlap( const AABB2& a, const AABB2& b );
AABB2 Interpolate( const AABB2& start, const AABB2& end, float fractionToward );

//bool DoDiscAndAABBOverlap( const Disc2& disc, const AABB2& aabb );
//bool DoDiscAndAABBOverlap( const AABB2& aabb, const Disc2& disc);