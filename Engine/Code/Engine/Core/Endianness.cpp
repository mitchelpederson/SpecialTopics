#include "Engine/Core/Endianness.hpp"

#include <algorithm>

typedef char byte_t;

//----------------------------------------------------------------------------------------------------------------
eEndianness GetEndiannessForCurrentPlatform() {
	unsigned int test = 1;

	unsigned char* byteArray = (unsigned char*) &test;
	if (byteArray[0] == 0x01) {
		return LITTLE_ENDIAN;
	}
	else {
		return BIG_ENDIAN;
	}
	
}


//----------------------------------------------------------------------------------------------------------------
void ToEndianness( size_t const size, void* data, eEndianness endianness ) {

	// If platform and target endianness is the same, early out
	if (endianness == GetEndiannessForCurrentPlatform()) {
		return;
	}

	// Swap all the bytes around
	byte_t* bytes = (byte_t*) data;
	unsigned int i = 0;
	unsigned int j = size - 1;

	while (i < j) {
		std::swap( bytes[i], bytes[j] );
		i++;
		j--;
	}
}


//----------------------------------------------------------------------------------------------------------------
void FromEndianness( size_t const size, void* data, eEndianness endianness ) {

	// if the given endianness and the target endianness are the same, early out
	if (endianness != GetEndiannessForCurrentPlatform()) {
		return;
	}

	// Swap all the bytes around
	byte_t* bytes = (byte_t*) data;
	unsigned int i = 0;
	unsigned int j = size - 1;

	while (i < j) {
		std::swap( bytes[i], bytes[j] );
		i++;
		j--;
	}
}