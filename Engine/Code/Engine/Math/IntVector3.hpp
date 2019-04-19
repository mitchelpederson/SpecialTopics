#pragma once 


class IntVector3 {
public:
	IntVector3();
	IntVector3( int x, int y, int z );



public:
	int x;
	int y;
	int z;
};

IntVector3 operator+( const IntVector3& a, const IntVector3& b );
IntVector3 operator-( const IntVector3& a, const IntVector3& b );