//----------------------------------------------------------------------------------------------------------------
// NetMessage.hpp
// Mitchel Pederson
//----------------------------------------------------------------------------------------------------------------

#pragma once 
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Net/NetAddress.hpp"

#include <string>


class NetMessage : public BytePacker {

public:

	NetMessage( std::string const& messageName, byte_t* messageData, size_t messageDataSize );

	// will write the message name to my BytePacker and then copy data from the given one
	NetMessage( std::string const& messageName, BytePacker const& packedData );
	NetMessage( uint8_t messageIndex );
	NetMessage( uint8_t messageIndex, byte_t* payload, size_t payloadSize, uint16_t reliableID = 0, uint16_t sequenceID = 0 );

	NetMessage( NetMessage const& copy );

	void SetTimeLastSent( float seconds );
	void SetReliableID( uint16_t reliableID );
	void SetSequenceID( uint16_t sequenceID );

	size_t GetMessageLength() const;		// Returns the size of the entire message
	std::string GetMessageName();			// Returns just the name
	BytePacker GetMessageData();			// Returns a BytePacker with the data segment copied into it
	uint8_t GetMessageIndex() const;
	
	bool IsReliable() const;
	bool IsInOrder() const;
	uint16_t GetReliableID() const;
	uint16_t GetSequenceID() const;
	float GetTimeLastSent() const;

private:
	NetAddress_T destination;
	uint8_t m_messageIndex;
	uint16_t m_reliableID = 0;
	uint16_t m_sequenceID = 0;
	float m_timeLastSent = 0.f;
};