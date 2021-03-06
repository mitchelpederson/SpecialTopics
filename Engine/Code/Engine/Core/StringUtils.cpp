#include "Engine/Core/StringUtils.hpp"
#include <stdarg.h>
#include <vector>


//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}


std::vector<std::string> SplitString( const std::string& toSplit, char delimiter ) {

	std::vector<std::string> tokens;
	int previousDelimIndex = 0;
	int currentIndex = 0;

	while (currentIndex < toSplit.size()) {
		if (toSplit[currentIndex] == delimiter) {
			tokens.push_back(toSplit.substr(previousDelimIndex, currentIndex - previousDelimIndex));
			if ( tokens.back() == "" ) {
				tokens.pop_back();
			}
			previousDelimIndex = currentIndex + 1;
		}
		currentIndex++;
	}

	std::string endToken = toSplit.substr(previousDelimIndex, currentIndex - previousDelimIndex);
	if ( endToken.size() > 0 ) {
		tokens.push_back( endToken );
	}

	return tokens;
}


std::string PrintUint16Binary( uint16_t num ) {
	std::string binary = "";

	for (int i = 15; i >= 0; i--) {
		int bit = num >> i;
		binary += std::to_string(bit & 1);
	}

	return binary;
}
