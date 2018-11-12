#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetSession.hpp"

//----------------------------------------------------------------------------------------------------------------
NetMessage::NetMessage( uint8_t messageIndex ) {
	m_messageIndex = messageIndex;
}


//----------------------------------------------------------------------------------------------------------------
NetMessage::NetMessage( uint8_t messageIndex, byte_t* payload, size_t payloadSize, uint16_t reliableID /* = 0 */, uint16_t sequenceID ) 
	: BytePacker(payloadSize, (void*) payload)
	, m_reliableID( reliableID )
	, m_sequenceID( sequenceID )
{
	m_messageIndex = messageIndex;
}


//----------------------------------------------------------------------------------------------------------------
NetMessage::NetMessage( NetMessage const& copy ) 
	: BytePacker( copy.GetWrittenByteCount(), copy.GetBuffer() )
{
	m_messageIndex = copy.GetMessageIndex();
	m_reliableID = copy.GetReliableID();
	m_sequenceID = copy.GetSequenceID();
}


//----------------------------------------------------------------------------------------------------------------
size_t NetMessage::GetMessageLength() const {
	return GetWrittenByteCount();
}


//----------------------------------------------------------------------------------------------------------------
std::string NetMessage::GetMessageName()  {
	ResetReadHead();

	char buffer[50];
	size_t size = ReadString( buffer, 49 );
	buffer[size] = '\0';

	std::string str = buffer;
	return str;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetMessage::GetMessageIndex() const {
	return m_messageIndex;
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetMessage::GetReliableID() const {
	return m_reliableID;
}

//----------------------------------------------------------------------------------------------------------------
bool NetMessage::IsReliable() const {
	return NetSession::GetCommand( m_messageIndex ).IsReliable();
}


//----------------------------------------------------------------------------------------------------------------
bool NetMessage::IsInOrder() const {
	return NetSession::GetCommand( m_messageIndex ).IsInOrder();
}


//----------------------------------------------------------------------------------------------------------------
float NetMessage::GetTimeLastSent() const {
	return m_timeLastSent;
}


//----------------------------------------------------------------------------------------------------------------
void NetMessage::SetTimeLastSent( float seconds ) {
	m_timeLastSent = seconds;
}


//----------------------------------------------------------------------------------------------------------------
void NetMessage::SetReliableID( uint16_t reliableID ) {
	m_reliableID = reliableID;
}


//----------------------------------------------------------------------------------------------------------------
void NetMessage::SetSequenceID( uint16_t sequenceID ) {
	m_sequenceID = sequenceID;
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetMessage::GetSequenceID() const {
	return m_sequenceID;
}