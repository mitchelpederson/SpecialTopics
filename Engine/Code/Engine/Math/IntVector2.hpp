
#pragma once
#include "Engine/Math/Vector2.hpp"
class IntVector2 {

public:

	IntVector2();
	IntVector2( const IntVector2& copyFrom );
	IntVector2( int newX, int newY );
	IntVector2( const Vector2& truncateFrom );
	~IntVector2();

	int x = 0;
	int y = 0;

	bool operator==(const IntVector2& compare);
	void SetFromText( const char* text );

	IntVector2 operator+(const IntVector2& add);
	IntVector2 operator-(const IntVector2& sub);
	IntVector2 RotateBy90Degrees() const;
	int GetDistance( const IntVector2& b ) const;
	
	static const IntVector2 NORTH;
	static const IntVector2 SOUTH;
	static const IntVector2 EAST;
	static const IntVector2 WEST;
	static const IntVector2 AWAYLEFT;	
	static const IntVector2 AWAYRIGHT;	
	static const IntVector2 TOWARDLEFT;	
	static const IntVector2 TOWARDRIGHT;


};


IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTwoards);