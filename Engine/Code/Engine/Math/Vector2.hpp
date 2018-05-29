#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2
{
public:
	// Construction/Destruction
	~Vector2() {}											// destructor: do nothing (for speed)
	Vector2() {}											// default constructor: do nothing (for speed)
	Vector2( const Vector2& copyFrom );						// copy constructor (from another vec2)
	explicit Vector2( float initialX, float initialY );		// explicit constructor (from x, y)


	// Math functions
			float	GetLength() const;
			float	GetLengthSquared() const;
			float	NormalizeAndGetLength();
			Vector2	GetNormalized() const;
			float	GetOrientationDegrees() const;
	static	Vector2	MakeDirectionAtDegrees( float degrees );


	// Constants


	// Operators
	const Vector2 operator+( const Vector2& vecToAdd ) const;		// vec2 + vec2
	const Vector2 operator-( const Vector2& vecToSubtract ) const;	// vec2 - vec2
	const Vector2 operator*( float uniformScale ) const;			// vec2 * float
	const Vector2 operator/( float inverseScale ) const;			// vec2 / float
	void operator+=( const Vector2& vecToAdd );						// vec2 += vec2
	void operator-=( const Vector2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );					// vec2 *= float
	void operator/=( const float uniformDivisor );					// vec2 /= float
	void operator=( const Vector2& copyFrom );						// vec2 = vec2
	bool operator==( const Vector2& compare ) const;				// vec2 == vec2
	bool operator!=( const Vector2& compare ) const;				// vec2 != vec2

	friend const Vector2 operator*( float uniformScale, const Vector2& vecToScale );	// float * vec2

	void SetFromText( const char* text );

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;


public:

	static const Vector2 NORTH;
	static const Vector2 SOUTH;
	static const Vector2 EAST;
	static const Vector2 WEST;
	static const Vector2 ZERO;
	static const Vector2 ZERO_TO_ONE;
};


// Standalone utilities
		Vector2	Reflect( const Vector2& vectorToReflect, const Vector2& normal );
const	Vector2	GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto );
const	Vector2	GetTransformedIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ );
const	Vector2	GetTransformedOutOfBasis( const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ );
		void	DecomposeVectorIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ, Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ );
		Vector2 Interpolate ( const Vector2& start, const Vector2& end, float fractionToward );
