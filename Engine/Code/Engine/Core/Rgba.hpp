#pragma once

class Rgba {

public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;


	Rgba();
	explicit Rgba( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );

	void SetAsBytes( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );
	void SetAsFloats( float redNormalized, float greenNormalized, float blueNormalized, float alphaNormalized );
	void GetAsFloats( float& out_redNormalized, float& out_greenNormalized, float& out_blueNormalized, float& out_alphaNormalized) const;
	void ScaleRGB( float rgbScale );
	void ScaleAlpha( float alphaScale );

	void SetFromText( const char* text );
	void SetFromHueDegrees( float degrees );

	Rgba operator*(float scale);
	bool operator==(const Rgba& other) const;

};


Rgba Interpolate( const Rgba& start, const Rgba& end, float timeInto );