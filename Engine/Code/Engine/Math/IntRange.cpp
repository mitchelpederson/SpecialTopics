#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <string>

IntRange::IntRange() 
	: min(0)
	, max(0)
{}


IntRange::IntRange(int initialMin, int initialMax) 
	: min(initialMin)
	, max(initialMax)
{}


IntRange::IntRange(int initialMinMax) 
	: min(initialMinMax)
	, max(initialMinMax)
{}


int IntRange::GetRandomInRange() const {
	return GetRandomIntInRange(min, max);
}


bool DoRangesOverlap( const IntRange& a, const IntRange& b ) {

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


bool IntRange::IsWithin( int i ) const {
	return i >= min && i <= max;
}

IntRange Interpolate ( const IntRange& start, const IntRange& end, float fractionToward ) {

	return IntRange( Interpolate(start.min, end.min, fractionToward), Interpolate(start.max, end.max, fractionToward));

}


void IntRange::SetFromText( const char* text ) 	{
	
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
		min = atoi(textCopy.substr(0, tildeIndex).c_str());
		max = atoi(textCopy.substr(tildeIndex + 1, textCopy.length() - 1).c_str());
	}
	
	else {
		min = atoi(textCopy.c_str());
		max = min;
	}
}
