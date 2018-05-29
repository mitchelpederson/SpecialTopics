#include "Engine\Math\AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>
#include <string.h>
#include "Engine/Core/StringUtils.hpp"


const AABB2 AABB2::ZERO_TO_ONE = AABB2(0.f, 0.f, 1.f, 1.f);

AABB2::AABB2() {
	mins = Vector2(0.f, 0.f);
	maxs = Vector2(1.f, 1.f);
}


AABB2::~AABB2() {

}


AABB2::AABB2( const AABB2& copy ) {
	mins = copy.mins;
	maxs = copy.maxs;
}


AABB2::AABB2( float minX, float minY, float maxX, float maxY ) {
	mins = Vector2(minX, minY);
	maxs = Vector2(maxX, maxY);
} 


AABB2::AABB2( const Vector2& minsCopy, const Vector2& maxsCopy ) {
	mins.x = minsCopy.x;
	mins.y = minsCopy.y;
	maxs.x = maxsCopy.x;
	maxs.y = maxsCopy.y;
}


AABB2::AABB2( const Vector2& center, float radiusX, float radiusY ) {
	mins = Vector2( center.x - radiusX, center.y - radiusY );
	maxs = Vector2( center.x + radiusX, center.y + radiusY );
}


void AABB2::StretchToIncludePoint( float x, float y ) {
	if ( IsPointInside(x, y) == false ) {
		if (x < mins.x) {
			mins.x = x;
		}
		if (y < mins.y) {
			mins.y = y;
		}
		if (x > maxs.x) {
			maxs.x = x;
		}
		if (y > maxs.y) {
			maxs.y = y;
		}
	}
}


void AABB2::StretchToIncludePoint( const Vector2& point ) {
	if ( IsPointInside(point) == false ) {
		if (point.x < mins.x) {
			mins.x = point.x;
		}
		if (point.y < mins.y) {
			mins.y = point.y;
		}
		if (point.x > maxs.x) {
			maxs.x = point.x;
		}
		if (point.y > maxs.y) {
			maxs.y = point.y;
		}
	}
}


void AABB2::AddPaddingToSides( float xPaddingRadius, float yPaddingRadius ) {
	mins.x -= xPaddingRadius;
	maxs.x += xPaddingRadius;
	mins.y -= yPaddingRadius;
	maxs.y += yPaddingRadius;

}


void AABB2::Translate( const Vector2& translation ) {
	mins += translation;
	maxs += translation;
}


void AABB2::Translate( float translationX, float translationY ) {
	mins.x += translationX;
	maxs.x += translationX;
	mins.y += translationY;
	maxs.y += translationY;
}


bool AABB2::IsPointInside( float x, float y ) const {

	if ( ( x > mins.x && x < maxs.x ) && ( y > mins.y && y < maxs.y ) ) {
		return true;
	}
	
	else {
		return false;
	}
}


bool AABB2::IsPointInside( const Vector2& point ) const {

	if ( ( point.x > mins.x && point.x < maxs.x ) && ( point.y > mins.y && point.y < maxs.y ) ) {
		return true;
	}

	else {
		return false;
	}
}


Vector2 AABB2::GetDimensions() const {
	return Vector2(maxs.x - mins.x, maxs.y - mins.y);
}


Vector2 AABB2::GetCenter() const {
	return Vector2( ( mins.x + maxs.x ) / 2.f, ( mins.y + maxs.y ) / 2.f );
}


void AABB2::operator+=( const Vector2& translation ) {
	mins += translation;
	maxs += translation;
}


void AABB2::operator-=( const Vector2& antiTranslation ) {
	mins -= antiTranslation;
	maxs -= antiTranslation;
}


AABB2 AABB2::operator+( const Vector2& translation ) const {
	return AABB2(mins + translation, maxs + translation);
}


AABB2 AABB2::operator-( const Vector2& antiTranslation ) const {
	return AABB2(mins - antiTranslation, maxs - antiTranslation);
}


bool DoAABBsOverlap( const AABB2& a, const AABB2& b) {


		// Check if an x axis or y axis from a are both inside b, doesn't matter which just that 
		// at least one x axis AND one y axis is inside a.
	if ( a.mins.x > b.maxs.x ||
		 a.maxs.x < b.mins.x || 
		 a.mins.y > b.maxs.y || 
		 a.maxs.y < b.mins.y) { //   a.maxs.y is inside b

		return false;
	}

	else {
		return true;
	}
}


AABB2 Interpolate( const AABB2& start, const AABB2& end, float fractionToward ) {

	Vector2 interpolatedMins( Interpolate(start.mins.x, end.mins.x, fractionToward), Interpolate(start.mins.y, end.mins.y, fractionToward));
	Vector2 interpolatedMaxs( Interpolate(start.maxs.x, end.maxs.x, fractionToward), Interpolate(start.maxs.y, end.maxs.y, fractionToward));


	return AABB2( Interpolate( start.mins, end.mins, fractionToward), Interpolate(start.maxs, end.maxs, fractionToward));
}


Vector2 AABB2::GetNearestPointOnBounds( const Vector2& outsidePoint ) {

	float nearestX = ClampFloat(outsidePoint.x, mins.x, maxs.x);
	float nearestY = ClampFloat(outsidePoint.y, mins.y, maxs.y);

	return Vector2(nearestX, nearestY);

	
}


void AABB2::SetFromText(const char* text) {
	std::vector<std::string> tokens = SplitString(text, ',');

	mins.x = (float) atof(tokens[0].c_str());
	mins.y = (float) atof(tokens[1].c_str());
	maxs.x = (float) atof(tokens[2].c_str());
	maxs.y = (float) atof(tokens[3].c_str());
}