#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <string>

FloatRange::FloatRange() 
	: min(0.f)
	, max(0.f)
{}


FloatRange::FloatRange(float initialMin, float initialMax) 
	: min(initialMin)
	, max(initialMax)
{}


FloatRange::FloatRange(float initialMinMax) 
	: min(initialMinMax)
	, max(initialMinMax)
{}


float FloatRange::GetRandomInRange() const {
	return GetRandomFloatInRange(min, max);
}


float FloatRange::Evaluate( float percent ) const {
	return Interpolate( min, max, percent );
}


bool DoRangesOverlap( const FloatRange& a, const FloatRange& b ) {

	if (a.min >= b.min && a.min <= b.max) {
		return true;
	}

	else if (a.max <= b.max && a.max >= b.min) {
		return true;
	}

	else {
		return false;
	}
}


FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowards) {

	return FloatRange( Interpolate(start.min, end.min, fractionTowards), Interpolate(start.max, end.max, fractionTowards));
}


void FloatRange::SetFromText( const char* text ) 	{

	int tildeIndex = 0;

	while (text[tildeIndex] != '~') {
		if (text[tildeIndex] == '\0') {
			tildeIndex = -1;
			break;
		}
		tildeIndex++;
	}

	std::string textCopy(text);

	if (tildeIndex > -1) {
		min = (float) atof(textCopy.substr(0, tildeIndex).c_str());
		max = (float) atof(textCopy.substr(tildeIndex + 1, textCopy.length() - 1).c_str());
	}

	else {
		min = (float) atof(textCopy.c_str());
		max = min;
	}
}