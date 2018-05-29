#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <string> 


const IntVector2 IntVector2::SOUTH	= IntVector2( 0,-1);
const IntVector2 IntVector2::NORTH	= IntVector2( 0, 1);
const IntVector2 IntVector2::EAST	= IntVector2(-1, 0);
const IntVector2 IntVector2::WEST	= IntVector2( 1, 0);
const IntVector2 IntVector2::AWAYLEFT		= IntVector2(-1,-1);
const IntVector2 IntVector2::AWAYRIGHT		= IntVector2( 1, 1);
const IntVector2 IntVector2::TOWARDLEFT		= IntVector2(-1, 1);
const IntVector2 IntVector2::TOWARDRIGHT	= IntVector2( 1,-1);


IntVector2::IntVector2() : x(0), y(0) {
	
}


IntVector2::IntVector2( const IntVector2& copyFrom ) : x(copyFrom.x), y(copyFrom.y) {

}


IntVector2::IntVector2( int newX, int newY ) : x(newX), y(newY) {

}


IntVector2::IntVector2( const Vector2& truncateFrom ) : x((int) truncateFrom.x), y((int) truncateFrom.y){

}


IntVector2::~IntVector2() {

}


bool IntVector2::operator==(const IntVector2& compare) {
	return (compare.x == x) && (compare.y == y);
}


IntVector2 IntVector2::operator-(const IntVector2& sub) {
	return IntVector2(x - sub.x, y - sub.y);
}

IntVector2 IntVector2::operator+(const IntVector2& add) {
	return IntVector2(x + add.x, y + add.y);
}


IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTwoards) {
	return IntVector2( Interpolate(start.x, end.x, fractionTwoards), Interpolate(start.y, end.y, fractionTwoards));
}


void IntVector2::SetFromText( const char* text ) {

	int commaIndex = 0;

	while (text[commaIndex] != ',') {
		commaIndex++;
	}

	std::string textCopy(text);

	x = atoi(textCopy.substr(0, commaIndex).c_str());
	y = atoi(textCopy.substr(commaIndex + 1, textCopy.length() - 1).c_str());

}


IntVector2 IntVector2::RotateBy90Degrees() const {
	return IntVector2( -y, x );
}

int IntVector2::GetDistance( const IntVector2& b ) const {
	return abs(x - b.x) + abs(y - b.y);
}