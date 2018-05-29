#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

int ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, int defaultValue ) {
	
	int readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute = atoi(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("int::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


char ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, char defaultValue ) {
	
	char readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute = attribute->Value()[0];
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("char::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


bool ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, bool defaultValue ) {
	
	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		if (strcmp(attribute->Value(),"true") == 0) {
			return true;
		}
		else if (strcmp(attribute->Value(), "false") == 0) {
			return false;
		}
		else {
			ERROR_RECOVERABLE("bool::ParseXmlAttribute was not \"true\" or \"false\"");
			return defaultValue;
		}
	}

	else {

		//ERROR_RECOVERABLE("bool::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


float ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, float defaultValue ) {
	float readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute = (float) atof(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("float::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


Rgba ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Rgba& defaultValue ) {

	Rgba readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute.SetFromText(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("Rgba::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


Vector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const Vector2& defaultValue ) {
	Vector2 readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute.SetFromText(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("Vector2::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}


}


IntRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntRange& defaultValue ) {

	IntRange readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute.SetFromText(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("IntRange::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


FloatRange ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const FloatRange& defaultValue ) {
	FloatRange readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute.SetFromText(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("FloatRange::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


IntVector2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const IntVector2& defaultValue ) {
	IntVector2 readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute.SetFromText(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("IntVector2::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}


AABB2 ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const AABB2& defaultValue ) {
	AABB2 readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute.SetFromText(attribute->Value());
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("IntVector2::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}



std::string ParseXmlAttribute( const tinyxml2::XMLElement& element, const char* attributeName, const std::string& defaultValue ) {
	
	std::string readAttribute;

	const tinyxml2::XMLAttribute* attribute = element.FindAttribute(attributeName);

	if (attribute != nullptr) {
		readAttribute = attribute->Value();
		return readAttribute;
	}

	else {

		//ERROR_RECOVERABLE("std::string::ParseXmlAttribute() could not find the given attribute name");
		return defaultValue;
	}
}
