#pragma once
#include "Engine/Core/Endianness.hpp"

//----------------------------------------------------------------------------------------------------------------
// Byte packer options bit defs
#define BIT_FLAG(f) (1U << (f))

enum eBytePackerOptionBit : unsigned int {
	BYTEPACKER_OWNS_MEMORY = BIT_FLAG(0),
	BYTEPACKER_CAN_GROW = BIT_FLAG(1) 
};

typedef unsigned int eBytePackerOptions;
typedef unsigned char byte_t;


//----------------------------------------------------------------------------------------------------------------
class BytePacker {
public:
	BytePacker( eEndianness endianness = LITTLE_ENDIAN, eBytePackerOptions options = (BYTEPACKER_OWNS_MEMORY | BYTEPACKER_CAN_GROW) );
	BytePacker( size_t bufferSize, eEndianness endianness = LITTLE_ENDIAN, eBytePackerOptions options = BYTEPACKER_OWNS_MEMORY );
	BytePacker( size_t bufferSize, void* buffer, eEndianness endianness = LITTLE_ENDIAN, eBytePackerOptions options = 0 );
	~BytePacker();

	void SetEndianness( eEndianness endianness );
	void SetReadableByteCount( size_t byteCount );

	bool WriteBytes( size_t byteCount, void const* data );
	size_t ReadBytes( void* out_data, size_t maxByteCount );
	size_t WriteSize( size_t size ); // returns bytes used
	size_t ReadSize( size_t* out_size ); // returns bytes read, fills out_size

	bool WriteString( char const* str );	// see notes for encoding
	size_t ReadString( char* out_str, size_t maxByteSize );

	void ResetWriteHead();		// Sets write head and read head to 0
	void ResetReadHead();		// Sets just the read head to 0


	eEndianness GetEndianness() const;
	size_t GetWrittenByteCount() const;
	size_t GetRemainingWritableByteCount() const;
	size_t GetRemainingReadableByteCount() const;


private:
	bool CanManageMemory() const;
	bool CanGrow() const;

	eBytePackerOptions m_options = 0;

	byte_t* m_data = nullptr;
	size_t m_dataByteCount = 0;

	size_t m_writeHeadByteIndex = 0;
	size_t m_readHeadByteIndex = 0;

	eEndianness m_endianness = LITTLE_ENDIAN;


};