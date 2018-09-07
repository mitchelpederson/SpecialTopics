#include "Engine/Core/BytePacker.hpp"

#include <string>

//----------------------------------------------------------------------------------------------------------------
BytePacker::BytePacker( eEndianness endianness /* = LITTLE_ENDIAN */, eBytePackerOptions options /* = (BYTEPACKER_OWNS_MEMORY | BYTEPACKER_CAN_GROW) */ ) 
	: m_endianness( endianness )
	, m_options( options )
{}


//----------------------------------------------------------------------------------------------------------------
BytePacker::BytePacker( size_t bufferSize, eEndianness endianness /* = LITTLE_ENDIAN */, eBytePackerOptions options /* = BYTEPACKER_OWNS_MEMORY */ ) 
	: m_endianness( endianness )
	, m_dataByteCount( bufferSize )
	, m_options( options )
{}


//----------------------------------------------------------------------------------------------------------------
BytePacker::BytePacker( size_t bufferSize, void* buffer, eEndianness endianness /* = LITTLE_ENDIAN */, eBytePackerOptions options /* = 0 */ ) 
	: m_endianness( endianness )
	, m_data( (byte_t*) buffer )
	, m_dataByteCount( bufferSize )
	, m_writeHeadByteIndex( bufferSize )
	, m_options( options )
{}


//----------------------------------------------------------------------------------------------------------------
BytePacker::~BytePacker() {
	if ( CanManageMemory() ) {
		delete[] m_data;
		m_data = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
void BytePacker::SetEndianness( eEndianness endianness ) {
	
}


//----------------------------------------------------------------------------------------------------------------
void BytePacker::SetReadableByteCount( size_t byteCount ) {
	//todo
}


//----------------------------------------------------------------------------------------------------------------
size_t BytePacker::WriteSize( size_t size ) {
	size_t remainder = size;
	byte_t sevenBitsMask = 0x7F;
	size_t bytesWritten = 0;

	while ( remainder != 0 ) {
		// Extract the seven bits, high bit will be 0
		byte_t extractedSevenBits = remainder & sevenBitsMask;

		// Shift the remainder seven bits
		remainder = remainder >> 7;

		// If the remainder is not 0, set the high bit to 1
		if (remainder != 0) {
			extractedSevenBits |= 0x80;
		}

		WriteBytes( 1, &extractedSevenBits );
		bytesWritten++;
	}

	return bytesWritten;
}


//----------------------------------------------------------------------------------------------------------------
size_t BytePacker::ReadSize( size_t* out_size ) {
	size_t& outSize = *out_size;
	outSize = 0;
	size_t bytesRead = 0;
	byte_t highBitMask = 0x80;
	size_t currentByte = 0x80;

	while ( (currentByte & highBitMask) == highBitMask ) {

		if ( ReadBytes( &currentByte, 1 ) == 0) {
			break;
		}
		
		size_t sizePortionToAdd = (currentByte & 0x7F);
		sizePortionToAdd = sizePortionToAdd << (bytesRead * 7);
		outSize = outSize | sizePortionToAdd;
		bytesRead++;
	}

	return bytesRead;	
}


//----------------------------------------------------------------------------------------------------------------
bool BytePacker::WriteBytes( size_t byteCount, void const* data ) {

	// if the data is within the max but greater than the currently allocated, allocate 
	if ( m_writeHeadByteIndex + byteCount >= m_dataByteCount ) {

		// If we can't grow, then failed to write.
		if (!CanGrow()) {
			return false;
		}

		byte_t* temp = m_data;

		m_data = new byte_t[ m_dataByteCount + byteCount ];
		memcpy( m_data, temp, m_dataByteCount );
		m_dataByteCount += byteCount;

		delete[] temp;
	}

	// Write the bytes
	byte_t* writeHeadPos = &(m_data[ m_writeHeadByteIndex ]);
	memcpy( writeHeadPos, data, byteCount );
	m_writeHeadByteIndex += byteCount;

	return true;
}


//----------------------------------------------------------------------------------------------------------------
size_t BytePacker::ReadBytes( void* out_data, size_t maxByteCount ) {
	
	// If there is less space than the maxByteCount, then reduce the max byte count to that amount
	if ( m_dataByteCount - m_readHeadByteIndex < maxByteCount ) {
		maxByteCount = m_dataByteCount - m_readHeadByteIndex;
	}

	// If the max byte count would extend past the write head, shrink it
	if (m_writeHeadByteIndex - m_readHeadByteIndex > GetRemainingWritableByteCount()) {
		maxByteCount = m_writeHeadByteIndex - m_readHeadByteIndex;
	}

	// Copy the maximum from the read head position
	byte_t* readHeadPos = &(m_data[ m_readHeadByteIndex ]);
	memcpy(out_data, readHeadPos, maxByteCount);
	m_readHeadByteIndex += maxByteCount;

	return maxByteCount;
}


//----------------------------------------------------------------------------------------------------------------
bool BytePacker::WriteString( char const* str ) {

	// Get the size of the string and then write the size
	size_t strSize = 0;
	while ( str[ strSize ] != '\0' ) {
		strSize++;
	} 
	WriteSize( strSize );

	// Now we can write the string itself.
	size_t strIndex = 0;
	while ( str[ strIndex ] != '\0' ) {
		unsigned char toWrite = str[ strIndex ];
		if ( !WriteBytes( 1, &toWrite ) ) {
			return false;
		}
		strIndex++;
	}
}


//----------------------------------------------------------------------------------------------------------------
size_t BytePacker::ReadString( char* out_str, size_t maxByteSize ) {
	size_t charIndex = 0;
	size_t length;
	ReadSize( &length );

	if ( length > maxByteSize ) {
		length = maxByteSize;
	}

	while ( charIndex < length ) {
		ReadBytes(&out_str[charIndex], 1);
		charIndex++;
	}
	out_str[charIndex] = '\0';
	return charIndex;
}


//----------------------------------------------------------------------------------------------------------------
void BytePacker::ResetReadHead() {
	m_readHeadByteIndex = 0;
}


//----------------------------------------------------------------------------------------------------------------
void BytePacker::ResetWriteHead() {
	m_writeHeadByteIndex = 0;
	ResetReadHead();
}


//----------------------------------------------------------------------------------------------------------------
bool BytePacker::CanGrow() const {
	if ( CanManageMemory() && ((m_options & BYTEPACKER_CAN_GROW) == BYTEPACKER_CAN_GROW) ) {
		return true;
	}
	return false;
}


//----------------------------------------------------------------------------------------------------------------
bool BytePacker::CanManageMemory() const {
	if ( (m_options & BYTEPACKER_OWNS_MEMORY) == BYTEPACKER_OWNS_MEMORY ) {
		return true;
	}
	return false;
}


//----------------------------------------------------------------------------------------------------------------
eEndianness BytePacker::GetEndianness() const {
	return m_endianness;
}


//----------------------------------------------------------------------------------------------------------------
size_t BytePacker::GetWrittenByteCount() const {
	return m_writeHeadByteIndex;
}


//----------------------------------------------------------------------------------------------------------------
size_t BytePacker::GetRemainingWritableByteCount() const {
	return m_dataByteCount - m_writeHeadByteIndex - 1;
}


//----------------------------------------------------------------------------------------------------------------
size_t BytePacker::GetRemainingReadableByteCount() const {
	return m_dataByteCount - m_readHeadByteIndex - 1;
}