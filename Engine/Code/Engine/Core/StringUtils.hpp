#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
std::vector<std::string> SplitString( const std::string& toSplit, char delimiter );
std::string PrintUint16Binary( uint16_t num );
