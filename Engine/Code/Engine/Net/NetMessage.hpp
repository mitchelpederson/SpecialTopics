//----------------------------------------------------------------------------------------------------------------
// NetMessage.hpp
// Mitchel Pederson
//
// Contains a message to be sent over the net. Basically a wrapper for a BytePacker but with a 
//  more limiting interface. 
// We will assume message data will be written into a BytePacker before a NetMessage object
//   is created so that we can put the responsibility for packing and unpacking on the user.
//
//----------------------------------------------------------------------------------------------------------------

#pragma once 
#include "Engine/Core/BytePacker.hpp"


class NetMessage {
	NetMessage( std::string const& messageName, byte_t* messageData, size_t messageDataSize );

	// will write the message name to my BytePacker and then copy data from the given one
	NetMessage( std::string const& messageName, BytePacker const& packedData );

	size_t GetMessageLength();		// Returns the size of the entire message
	size_t GetMessageDataSize();	// Returns how long the data is
	std::string GetMessageName();	// Returns just the name
	BytePacker GetMessageData();	// Returns a BytePacker with the data segment copied into it

	void* GetDataToBeSent();		// Returns BytePacker's data as a void* so we can send it to a socket
	

private:
	BytePacker m_message;
};