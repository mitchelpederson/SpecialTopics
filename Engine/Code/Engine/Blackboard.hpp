#pragma once
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include <map>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/FloatRange.hpp"

class Blackboard {

public:
	Blackboard(const std::string& filePath);
	~Blackboard();

	void			PopulateFromXmlElementAttributes( const tinyxml2::XMLElement& element );
	void			SetValue( const std::string& keyName, const std::string& newValue );

	bool			GetValue( const std::string& keyName, bool defaultValue ) const;
	int				GetValue( const std::string& keyName, int defaultValue ) const;
	float			GetValue( const std::string& keyName, float defaultValue ) const;
	std::string		GetValue( const std::string& keyName, std::string defaultValue ) const;
	std::string		GetValue( const std::string& keyName, const char* defaultValue ) const;
	Rgba			GetValue( const std::string& keyName, const Rgba& defaultValue ) const;
	Vector2			GetValue( const std::string& keyName, const Vector2& defaultValue ) const;
	IntVector2		GetValue( const std::string& keyName, const IntVector2& defaultValue ) const;
	FloatRange		GetValue( const std::string& keyName, const FloatRange& defaultValue ) const;
	IntRange		GetValue( const std::string& keyName, const IntRange& defaultValue ) const;


private:
	std::string gameConfigFilePath;
	std::map<std::string, std::string> m_blackboard;
	tinyxml2::XMLDocument* m_blackboardXMLDocument;
};