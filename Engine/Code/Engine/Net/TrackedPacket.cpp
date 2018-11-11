#include "Engine/Net/TrackedPacket.hpp"

#include "Engine/Core/EngineCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
void TrackedPacket::SetPacket( NetPacket* packet ) {
	m_packet = packet;
	m_timeSent = g_masterClock->total.seconds;
}


//----------------------------------------------------------------------------------------------------------------
float TrackedPacket::GetTimeSent() {
	return m_timeSent;
}


//----------------------------------------------------------------------------------------------------------------
void TrackedPacket::SetTimeSent( float seconds ) {
	m_timeSent = seconds;
}


//----------------------------------------------------------------------------------------------------------------
void TrackedPacket::Invalidate() {
	m_isValid = false;
}


//----------------------------------------------------------------------------------------------------------------
bool TrackedPacket::IsValid() {
	return m_isValid;
}


//----------------------------------------------------------------------------------------------------------------
void TrackedPacket::AddSentReliable( uint16_t id ) {
	if ( m_reliablesInPacket < 32 ) {
		m_sentReliables[ m_reliablesInPacket ] = id;
		m_reliablesInPacket++;
	}
}


//----------------------------------------------------------------------------------------------------------------
uint8_t TrackedPacket::GetNumReliablesInPacket() {
	return m_reliablesInPacket;
}


//----------------------------------------------------------------------------------------------------------------
uint16_t* TrackedPacket::GetSentReliablesArray() {
	return m_sentReliables;
}