#include "Engine/Blackboard.hpp"
#include <string>

Blackboard::Blackboard(const std::string& path) : gameConfigFilePath(path) {

	m_blackboardXMLDocument = new tinyxml2::XMLDocument();
	m_blackboardXMLDocument->LoadFile(gameConfigFilePath.c_str());

	tinyxml2::XMLElement* root = m_blackboardXMLDocument->FirstChildElement("GameConfig");

	PopulateFromXmlElementAttributes(*root);
}


Blackboard::~Blackboard() {
	delete m_blackboardXMLDocument;
}


void Blackboard::PopulateFromXmlElementAttributes( const tinyxml2::XMLElement& element ) {

	const tinyxml2::XMLAttribute* current = element.FirstAttribute();

	while (current != nullptr) {

		m_blackboard[current->Name()] = current->Value();

		current = current->Next();
	}
}


void Blackboard::SetValue( const std::string& keyName, const std::string& newValue ) {
	m_blackboard[keyName] = newValue;
}


bool Blackboard::GetValue( const std::string& keyName, bool defaultValue ) const {

	std::string key = keyName;

	if (m_blackboard.find(key) != m_blackboard.end()) {

		if (m_blackboard.at(key) == "true"){
			return true;
		}
		else if (m_blackboard.at(key) == "false") {
			return false;
		}
		else {
			return defaultValue;
		}

	}
	else {
		return defaultValue; 
	}
}


int	Blackboard::GetValue( const std::string& keyName, int defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		return atoi(m_blackboard.at(key).c_str());
	}
	else {
		return defaultValue; 
	}
}


float Blackboard::GetValue( const std::string& keyName, float defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		return (float) atof(m_blackboard.at(key).c_str());
	}
	else {
		return defaultValue; 
	}
}


std::string	Blackboard::GetValue( const std::string& keyName, std::string defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		return m_blackboard.at(key);
	}
	else {
		return defaultValue; 
	}
}


std::string	Blackboard::GetValue( const std::string& keyName, const char* defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		return m_blackboard.at(key);
	}
	else {
		return defaultValue; 
	}
}


Rgba Blackboard::GetValue( const std::string& keyName, const Rgba& defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		Rgba value;
		value.SetFromText(m_blackboard.at(key).c_str());
		return value;
	}
	else {
		return defaultValue; 
	}
}


Vector2	Blackboard::GetValue( const std::string& keyName, const Vector2& defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		Vector2 value;
		value.SetFromText(m_blackboard.at(key).c_str());
		return value;
	}
	else {
		return defaultValue; 
	}
}


IntVector2 Blackboard::GetValue( const std::string& keyName, const IntVector2& defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		IntVector2 value;
		value.SetFromText(m_blackboard.at(key).c_str());
		return value;
	}
	else {
		return defaultValue; 
	}
}


FloatRange Blackboard::GetValue( const std::string& keyName, const FloatRange& defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		FloatRange value;
		value.SetFromText(m_blackboard.at(key).c_str());
		return value;
	}
	else {
		return defaultValue; 
	}
}


IntRange Blackboard::GetValue( const std::string& keyName, const IntRange& defaultValue ) const {
	std::string key = keyName;
	if (m_blackboard.find(key) != m_blackboard.end()) {
		IntRange value;
		value.SetFromText(m_blackboard.at(key).c_str());
		return value;
	}
	else {
		return defaultValue; 
	}
}

