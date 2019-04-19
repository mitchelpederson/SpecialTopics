#pragma once
class FloatRange {
public:

	FloatRange();
	FloatRange(float initialMin, float initialMax);
	FloatRange(float initialMinMax);


	float GetRandomInRange() const;
	float Evaluate( float percent ) const;

	float min;
	float max;
	void SetFromText( const char* text );

};

bool DoRangesOverlap( const FloatRange& a, const FloatRange& b);
FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowards);