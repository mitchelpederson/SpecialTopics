#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"


Rgba::Rgba() : r(255), g(255), b(255), a(255) 
{
}


Rgba::Rgba( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha ) :
	r(red), 
	g(green), 
	b(blue), 
	a(alpha)
{}


void Rgba::SetAsBytes( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha ) {
	r = red;
	g = green;
	b = blue;
	a = alpha;
}



void Rgba::SetAsFloats( float redNormalized, float greenNormalized, float blueNormalized, float alphaNormalized ) {
	r = (unsigned char) (redNormalized * 255.f);
	g = (unsigned char) (greenNormalized * 255.f);
	b = (unsigned char) (blueNormalized * 255.f);
	a = (unsigned char) (alphaNormalized * 255.f);
}



void Rgba::GetAsFloats( float& out_redNormalized, float& out_greenNormalized, float& out_blueNormalized, float& out_alphaNormalized) const {
	out_redNormalized = (float) r / (float) 255;
	out_greenNormalized = (float) g / (float) 255;
	out_blueNormalized = (float) b / (float) 255;
	out_alphaNormalized = (float) a / (float) 255;
}


void Rgba::ScaleRGB( float rgbScale ) {

	float redFloat = (float) r / 255.f;
	float greenFloat = (float) g / 255.f;
	float blueFloat = (float) b / 255.f;

	float redScaled = redFloat * rgbScale;
	float greenScaled = greenFloat * rgbScale;
	float blueScaled = blueFloat * rgbScale;

	r = (unsigned char) (redScaled * 255.f);
	g = (unsigned char) (greenScaled * 255.f);
	b = (unsigned char) (blueScaled * 255.f);

}


void Rgba::ScaleAlpha( float alphaScale ) {
	float alphaFloat = (float) a / 255.f;

	float alphaScaled = alphaFloat * alphaScale;

	a = (unsigned char) (alphaScaled *  255.f);
}

Rgba Rgba::operator*(float scale) {

	Rgba colorNormalized(r, g, b, a);

	colorNormalized.ScaleAlpha(scale);
	colorNormalized.ScaleRGB(scale);

	return colorNormalized;
}


bool Rgba::operator==(const Rgba& other) const {
	if (other.r == r && other.g == g && other.a == a && other.b == b) {
		return true;
	}

	return false;
}


void Rgba::SetFromText( const char* text ) {

	int commaCount = 0;
	int index = 0;

	while (text[index] != '\0') {

		if (text[index] == ',') {
			commaCount++;
		}

		index++;
	}

	int indexOfFirstComma = 0;

	while (text[indexOfFirstComma] != ',') {
		indexOfFirstComma++;
	}

	int indexOfSecondComma = indexOfFirstComma + 1;

	while (text[indexOfSecondComma] != ',') {
		indexOfSecondComma++;
	}
	
	int indexOfThirdComma = indexOfSecondComma + 1;

	while (text[indexOfThirdComma] != ',') {

		if (text[indexOfThirdComma] == '\0') {
			indexOfThirdComma = -1;
			break;
		}
		
		indexOfThirdComma++;
	}

	std::string textCopy(text);

	if (indexOfThirdComma > 0) {

		std::string firstPart  =	textCopy.substr(0, indexOfFirstComma);
		std::string secondPart =	textCopy.substr(indexOfFirstComma + 1, indexOfSecondComma - indexOfFirstComma);
		std::string thirdPart  =	textCopy.substr(indexOfSecondComma + 1, indexOfThirdComma - indexOfSecondComma);
		std::string fourthPart =	textCopy.substr(indexOfThirdComma + 1, textCopy.length() - indexOfThirdComma);

		r = (unsigned char) atoi(firstPart.c_str());
		g = (unsigned char) atoi(secondPart.c_str());
		b = (unsigned char) atoi(thirdPart.c_str());
		a = (unsigned char) atoi(fourthPart.c_str());

	}

	else {
		std::string firstPart  =	textCopy.substr(0, indexOfFirstComma);
		std::string secondPart =	textCopy.substr(indexOfFirstComma + 1, (indexOfSecondComma - indexOfFirstComma) - 1);
		std::string thirdPart  =	textCopy.substr(indexOfSecondComma + 1, (textCopy.length() - indexOfSecondComma) - 1);

		r = (unsigned char) atoi(firstPart.c_str());
		g = (unsigned char) atoi(secondPart.c_str());
		b = (unsigned char) atoi(thirdPart.c_str());
	}
}


Rgba Interpolate( const Rgba& start, const Rgba& end, float timeInto ) {
	return Rgba( Interpolate(start.r, end.r, timeInto)
		, Interpolate(start.g, end.g, timeInto)
		, Interpolate(start.b, end.b, timeInto)	
		, Interpolate(start.a, end.a, timeInto)
	);
}


void Rgba::SetFromHueDegrees( float degrees ) {
	float c = 1.f;
	float x = c * ( 1.f - abs( fmodf((degrees / 60.f), 2.f) - 1 ) );
	
	if (degrees >= 0.f && degrees < 60.f) {
		r = (unsigned char) (c * 255.f);
		g = (unsigned char) (x * 255.f);
		b = 0;
		a = 255;
	}
	else if (degrees >= 60.f && degrees < 120.f) {
		r = (unsigned char) (x * 255.f);
		g = (unsigned char) (c * 255.f);
		b = 0;
		a = 255;
	}
	else if (degrees >= 120.f && degrees < 180.f) {
		r = 0;
		g = (unsigned char) (c * 255.f);
		b = (unsigned char) (x * 255.f);
		a = 255;
	}
	else if (degrees >= 180.f && degrees < 240.f) {
		r = 0;
		g = (unsigned char) (x * 255.f);
		b = (unsigned char) (c * 255.f);
		a = 255;
	}
	else if (degrees >= 240.f && degrees < 300.f) {
		r = (unsigned char) (x * 255.f);
		g = 0;
		b = (unsigned char) (c * 255.f);
		a = 255;
	}
	else if (degrees >= 300.f && degrees < 360.f) {
		r = (unsigned char) (c * 255.f);
		g = 0;
		b = (unsigned char) (x * 255.f);
		a = 255;
	}
}