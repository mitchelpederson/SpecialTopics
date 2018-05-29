#pragma once


class IntRange {
public:

	IntRange();
	IntRange(int initialMin, int initialMax);
	IntRange(int initialMinMax);

	int GetRandomInRange() const;
	bool IsWithin( int i ) const;

	int min;
	int max;
	void SetFromText( const char* text );

};


bool DoRangesOverlap( const IntRange& a, const IntRange& b );
IntRange Interpolate( const IntRange& start, const IntRange& end, float fractionTowards);