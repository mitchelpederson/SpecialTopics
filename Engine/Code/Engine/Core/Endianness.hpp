#pragma once


enum eEndianness {
	LITTLE_ENDIAN,
	BIG_ENDIAN
};


eEndianness GetEndiannessForCurrentPlatform();

void ToEndianness( size_t const size, void* data, eEndianness endianness );
void FromEndianness( size_t const size, void* data, eEndianness endianness );