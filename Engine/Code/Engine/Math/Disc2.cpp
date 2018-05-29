#include "Engine\Math\Disc2.hpp"
#include "Engine\Math\MathUtils.hpp"


Disc2::Disc2() {
	center = Vector2(0.f, 0.f);
	radius = 0.f;
}


Disc2::~Disc2() {

}


Disc2::Disc2( const Disc2& copyFrom ) {
	center = copyFrom.center;
	radius = copyFrom.radius;
}


Disc2::Disc2( float initialX, float initialY, float initialRadius ) {
	center = Vector2( initialX, initialY );
	radius = initialRadius;
}


Disc2::Disc2( const Vector2& initialCenter, float initialRadius ) {
	center = initialCenter;
	radius = initialRadius;
}


void Disc2::StretchToIncludePoint( float x, float y ) {

	if (GetDistance(center, Vector2(x, y)) > radius) {
		radius = GetDistance(center, Vector2(x, y));
	}

}


void Disc2::StretchToIncludePoint( const Vector2& point ) {

	if (GetDistance(center, point) > radius) {
		radius = GetDistance(center, point);
	}
}


void Disc2::AddPaddingToRadius( float paddingRadius ) {
	radius += paddingRadius;
}


void Disc2::Translate( const Vector2& translation ) {
	center += translation;
}


void Disc2::Translate( float translationX, float translationY ) {
	center.x += translationX;
	center.y += translationY;
}


bool Disc2::IsPointInside( float x, float y ) const { 

	if (GetDistance(center, Vector2(x, y)) > radius ) {
		return false;
	}
	else {
		return true;
	}
}


bool Disc2::IsPointInside( const Vector2& point ) const { 

	if (GetDistance(center, point) > radius ) {
		return false;
	}
	else {
		return true;
	}
}


void Disc2::operator+=( const Vector2& translation ) {
	center += translation;
}


void Disc2::operator-=( const Vector2& antiTranslation ) {
	center -= antiTranslation;
} 


Disc2 Disc2::operator+( const Vector2& translation ) const {
	return Disc2( center + translation, radius );
}


Disc2 Disc2::operator-( const Vector2& antiTranslation ) const {
	return Disc2( center + antiTranslation, radius );
}


bool DoDiscsOverlap( const Disc2& a, const Disc2& b ) {

	if ( GetDistanceSquared( a.center, b.center) < (a.radius + b.radius) * (a.radius + b.radius) ) {
		return true;
	}

	else {
		return false;
	}
}


bool DoDiscsOverlap( const Vector2& aCenter, float aRadius,
					 const Vector2& bCenter, float bRadius ) {

	if ( GetDistanceSquared( aCenter, bCenter) < (aRadius + bRadius) * (aRadius + bRadius) ) {
		return true;
	}

	else {
		return false;
	}
}


Disc2 Interpolate ( const Disc2& start, const Disc2& end, float fractionToward ) {

	return Disc2(Interpolate(start.center, end.center, fractionToward), Interpolate(start.radius, end.radius, fractionToward));
}

