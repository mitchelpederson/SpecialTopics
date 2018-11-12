#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/NetSession.hpp"


//----------------------------------------------------------------------------------------------------------------
NetPacket::NetPacket() {
}


//----------------------------------------------------------------------------------------------------------------
NetPacket::NetPacket( void* buffer, size_t length ) : BytePacker( ) {
	WriteBytes( length, buffer );
}


//----------------------------------------------------------------------------------------------------------------
NetPacket::~NetPacket() {

}


//----------------------------------------------------------------------------------------------------------------
void NetPacket::WriteHeader( NetPacketHeader_T header ) {
	WriteBytesAt( 1, &header.connectionIndex, 0 );
	WriteValueAt<uint16_t>( header.ack, 1 );
	WriteValueAt<uint16_t>( header.lastRecvdAck, 3 );
	WriteValueAt<uint16_t>( header.previousRecvdAckBitfield, 5 );
	WriteBytesAt( 1, &header.messageCount, 7 );
}


//----------------------------------------------------------------------------------------------------------------
void NetPacket::WriteMessage( NetMessage const& message ) {

	uint8_t headerSize;

	if ( message.IsInOrder() ) {
		headerSize = 5;
	} else if ( message.IsReliable() ) {
		headerSize = 3;
	} else {
		headerSize = 1;
	}

	WriteValue<uint16_t>( message.GetMessageLength() + headerSize );
	WriteValue<uint8_t>( message.GetMessageIndex() );
	if ( message.IsReliable() ) {
		WriteValue<uint16_t>( message.GetReliableID() );
	}
	if ( message.IsInOrder() ) {
		WriteValue<uint16_t>( message.GetSequenceID() );
	}
	WriteBytes( message.GetMessageLength(), message.GetBuffer() );
}


//----------------------------------------------------------------------------------------------------------------
void NetPacket::ReadHeader( NetPacketHeader_T& out_header ) {
	ReadBytes( &out_header.connectionIndex, 1 );
	ReadValue<uint16_t>( &out_header.ack );
	ReadValue<uint16_t>( &out_header.lastRecvdAck );
	ReadValue<uint16_t>( &out_header.previousRecvdAckBitfield );
	ReadBytes( &out_header.messageCount, 1 );
}


//----------------------------------------------------------------------------------------------------------------
NetMessage* NetPacket::ReadMessage( NetSession* session ) {
	uint16_t size;
	ReadValue<uint16_t>( &size );

	uint8_t messageIndex;
	ReadValue<uint8_t>( &messageIndex );

	if ( NetSession::GetCommand( messageIndex ).IsInOrder() ) {
		uint16_t reliableID;
		ReadValue<uint16_t>( &reliableID );

		uint16_t sequenceID;
		ReadValue<uint16_t>( &sequenceID );

		char* buffer = new char[size - 5U];
		ReadBytes( buffer, size - 5U );

		NetMessage* message = new NetMessage( messageIndex, (byte_t*) buffer, size - 5U, reliableID, sequenceID );
		return message;
	}

	else if ( NetSession::GetCommand( messageIndex ).IsReliable() ) {

		uint16_t reliableID;
		ReadValue<uint16_t>( &reliableID );

		char* buffer = new char[size - 3U];
		ReadBytes( buffer, size - 3U );

		NetMessage* message = new NetMessage( messageIndex, (byte_t*) buffer, size - 3U, reliableID );
		return message;
	}

	else {
		char* buffer = new char[size - 1U];
		ReadBytes( buffer, size - 1U );

		NetMessage* message = new NetMessage( messageIndex, (byte_t*) buffer, size - 1U );
		return message;
	}
}