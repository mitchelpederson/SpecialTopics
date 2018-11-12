#pragma once

#include "Engine/Net/NetConnection.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/EngineCommon.hpp"

//----------------------------------------------------------------------------------------------------------------
NetConnection::NetConnection( NetSession* session, uint8_t connectionIndex, NetAddress_T const& address )
	: m_remoteAddress( address )
	, m_connectionIndex( connectionIndex )
	, m_session( session )
	, m_sendTick( g_masterClock )
	, m_heartbeat( g_masterClock )
{
	m_sendTick.SetTimer( 0.1f );
	m_heartbeat.SetTimer( DEFAULT_HEARTBEAT );
	m_timeAtLastReceive = g_masterClock->total.seconds;
}


//----------------------------------------------------------------------------------------------------------------
NetConnection::~NetConnection() {

}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::UpdateHeartbeat() {
	if (m_heartbeat.CheckAndReset()) {
		NetMessage heartbeat( m_session->GetMessageIndexForName( "heartbeat" ) );
		Send( heartbeat );
	}
}


//----------------------------------------------------------------------------------------------------------------
int NetConnection::SendPacket( UDPSocket* socketToSendFrom ) {
	
	// Early out if nothing needs to be sent
	if ( m_outgoingUnreliables.size() == 0 && m_unconfirmedReliables.size() == 0 && m_unsentReliables.size() == 0 ) {
		return 0;
	}

	// Write the packet header
	NetPacket* packet = new NetPacket();
	NetPacketHeader_T packetHeader;

	packetHeader.connectionIndex = m_session->GetMyConnectionIndex();
	packetHeader.messageCount = 0;
	packetHeader.ack = GetNextAckToSend();
	packetHeader.lastRecvdAck = m_highestRecvdAck;
	packetHeader.previousRecvdAckBitfield = m_previousRecvdAckBitfield;
	packet->WriteHeader( packetHeader ); // We should write the header to reserve the space in the buffer

	TrackedPacket* trackedPacket = AddTrackedPacket( packet, packetHeader.ack );

	int reliablesInPacket = 0;

	//-----
	// Unconfirmed reliables
	if ( m_unconfirmedReliables.size() > 0 && packet->GetWrittenByteCount() < MTU) {
		for ( int i = 0; i < m_unconfirmedReliables.size(); i++ ) {

			if (reliablesInPacket >= MAX_RELIABLES_PER_PACKET) {
				break;
			}

			if ( g_masterClock->total.seconds - m_unconfirmedReliables[i]->GetTimeLastSent() > UNRELIABLE_RESEND_TIME ) {
				
				if (m_unconfirmedReliables[i]->GetWrittenByteCount() + (m_unconfirmedReliables[i]->IsInOrder() ? 7 : 5) + packet->GetWrittenByteCount() >= MTU) {
					break;
				}

				packet->WriteMessage( *m_unconfirmedReliables[i] );
				reliablesInPacket++;
				packetHeader.messageCount++;
				m_unconfirmedReliables[i]->SetTimeLastSent( g_masterClock->total.seconds );
				trackedPacket->AddSentReliable( m_unconfirmedReliables[i]->GetReliableID() );
			}
		}
	}

	//-----
	// Unsent reliables
	if ( m_unsentReliables.size() > 0 && packet->GetWrittenByteCount() < MTU) {
		while ( !m_unsentReliables.empty() && CanSendNewReliable() ) {
		
			NetMessage* msg = m_unsentReliables.front(); 

			msg->SetReliableID( m_lastSentReliable + 1 );
			m_lastSentReliable++;

			if (reliablesInPacket > MAX_RELIABLES_PER_PACKET || packet->GetWrittenByteCount() + msg->GetWrittenByteCount() + (msg->IsInOrder() ? 7 : 5) >= MTU ) {
				break;
			}

			packet->WriteMessage( *msg );
			m_unconfirmedReliables.push_back( msg );
			m_unconfirmedReliables[ m_unconfirmedReliables.size() - 1 ]->SetTimeLastSent( g_masterClock->total.seconds );
			reliablesInPacket++;
			packetHeader.messageCount++;
			trackedPacket->AddSentReliable( msg->GetReliableID() );
			m_unsentReliables.pop();
		}
 	}

	//-----
	// Unreliables
	if ( m_outgoingUnreliables.size() > 0 && packet->GetWrittenByteCount() < MTU ) {

		// Write messages to the packet
		while ( !m_outgoingUnreliables.empty() ) {
			packet->WriteMessage( *m_outgoingUnreliables.front() );
			packetHeader.messageCount++;
			m_outgoingUnreliables.pop();
		}
	}

	packet->WriteHeader( packetHeader ); // Write the real values over that

	// Send the packet 
	int sentBytes = socketToSendFrom->SendTo( m_remoteAddress, packet->GetBuffer(), packet->GetWrittenByteCount() );

	m_timeAtLastSend = g_masterClock->total.seconds;
	IncrementNextAckToSend();

	return sentBytes;
}


//----------------------------------------------------------------------------------------------------------------
int NetConnection::SendPacketImmediate( UDPSocket* socketToSendFrom, NetMessage& message, bool isAckConfirm /* = false */ ) {

	NetPacket* packet = new NetPacket();
	NetPacketHeader_T packetHeader;
	packetHeader.connectionIndex = m_session->GetMyConnectionIndex();

	if (!isAckConfirm) {
		packetHeader.ack = GetNextAckToSend();
		packetHeader.messageCount = 1;
	}
	else {
		packetHeader.ack = INVALID_PACKET_ACK;
		packetHeader.messageCount = 0;
	}

	packetHeader.lastRecvdAck = m_highestRecvdAck;
	packetHeader.previousRecvdAckBitfield = m_previousRecvdAckBitfield;

	packet->WriteHeader( packetHeader );
	TrackedPacket* trackedPacket = AddTrackedPacket( packet, packetHeader.ack );

	if (!isAckConfirm) {

		if ( message.IsReliable() && CanSendNewReliable() ) {
			NetMessage* msg = new NetMessage( message );
			msg->SetReliableID(m_lastSentReliable + 1);
			m_lastSentReliable++;
			m_unconfirmedReliables.push_back( msg );
			trackedPacket->AddSentReliable( m_lastSentReliable );
		}
		packet->WriteMessage( message );
	}

	m_timeAtLastSend = g_masterClock->total.seconds;

	if (!isAckConfirm) {
		IncrementNextAckToSend();
	}

	return socketToSendFrom->SendTo( m_remoteAddress, packet->GetBuffer(), packet->GetWrittenByteCount() );
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::Send( NetMessage& message ) {

	NetMessage* msg = new NetMessage( message );
	if ( msg->IsInOrder() ) {
		SetSequenceIDOnMessage( msg );
	}
	if ( msg->IsReliable() ) {
		m_unsentReliables.push( msg );
	} else {
		m_outgoingUnreliables.push( msg );
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::Receive( NetMessage* message ) {
	m_incomingMessages.push( message );
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::ProcessIncoming( NetPacket& packet ) {

	// First we will validate the packet.
	packet.ResetReadHead();
	NetPacketHeader_T packetHeader;
	packet.ReadHeader( packetHeader );

	int actualSize = 8;
	for ( int i = 0; i < packetHeader.messageCount; i++) {
		NetMessage* message = packet.ReadMessage( m_session );

		if ( message->IsInOrder() ) {
			actualSize += message->GetMessageLength() + 7;
		} else if ( message->IsReliable() ) {
			actualSize += message->GetMessageLength() + 5;
		} else {
			actualSize += message->GetMessageLength() + 3;
		}
	}

	if (actualSize != packet.GetWrittenByteCount()) {
		DevConsole::Printf("Packet size did not match the size of all messages together - must be bad");
		return;
	}


	// Confirm any tracked packets we know the other side got from the history
	if ( packetHeader.lastRecvdAck != INVALID_PACKET_ACK ) {
		ConfirmPacketReceived( packetHeader.lastRecvdAck );
		for ( unsigned int i = 0; i < 16; i++ ) {
			uint16_t flag = 1 << i;
			if ( packetHeader.previousRecvdAckBitfield & flag ) {
				ConfirmPacketReceived( packetHeader.lastRecvdAck - i );
			}
		}
	}

	// If the received packet has a valid ack (meaning it's tracked) we have
	// to send a confirmation message and update m_highestRecvdAck and m_previousRecvdAckBitfield
	if ( packetHeader.ack != INVALID_PACKET_ACK ) {
		NetMessage dummy( m_session->GetMessageIndexForName("pong"), nullptr, 0 );
		SendPacketImmediate( m_session->GetSocket(), dummy, true );


		uint16_t difference = packetHeader.ack - m_highestRecvdAck;

		// If the recvd ack is newer than our previous one
		if ( difference < (0xFFFF / 2) ) {

			m_previousRecvdAckBitfield = m_previousRecvdAckBitfield << difference;
			m_previousRecvdAckBitfield = m_previousRecvdAckBitfield + 1;

			m_highestRecvdAck = packetHeader.ack;

		} 

		// Or if the recvd ack is older than our most recent
		else {
			uint16_t dist = m_highestRecvdAck - packetHeader.ack;
			m_previousRecvdAckBitfield |= 1 << dist;
		}
	}

	// Actually process the packet now.
	packet.ResetReadHead();
	packet.ReadHeader( packetHeader );

	for ( int i = 0; i < packetHeader.messageCount; i++ ) {
		NetMessage* message = packet.ReadMessage( m_session );

		if ( message == nullptr ) {
			delete message;
			message = nullptr;
			DevConsole::Printf("A message was malformed");
			break;
		}

		NetCommand messageCommand = NetSession::GetCommand( message->GetMessageIndex() );
		if (messageCommand.id != 0xFF) {
			if ( !m_session->IsValidConnectionIndex( m_connectionIndex ) && messageCommand.RequiresConnection() ) {
				DevConsole::Printf( "Message from someone unconnected requires a connection!!" );
			}
			else if ( m_session->IsValidConnectionIndex( m_connectionIndex ) || !messageCommand.RequiresConnection() ) {
				
				if ( message->IsReliable() ) {
					std::list<uint16_t>::iterator it = m_receivedReliables.begin();
					while ( it != m_receivedReliables.end() && *it != message->GetReliableID() ) {
						it++;
					}

					if ( it == m_receivedReliables.end() ) {

						if ( m_highestReceivedReliable - message->GetReliableID() < RELIABLE_WINDOW ) {
							
							if ( message->IsInOrder() ) {
								NetMessageChannel& channel = GetChannelForMessage( message );
								
								if ( message->GetSequenceID() == channel.m_nextExpectedSequenceID ) {
									messageCommand.callback( *message, *this );
									channel.m_nextExpectedSequenceID++;
								} 

								else {
									channel.m_outOfOrderMessages.push_back( message );
									AddToReceivedReliablesList( message->GetReliableID() );
								}
							}

							else {
								messageCommand.callback( *message, *this );
								AddToReceivedReliablesList( message->GetReliableID() );
							}
						}
					}
				}
				else {
					messageCommand.callback( *message, *this );
				}
			}
		}
		else {
			DevConsole::Printf("Message index was not valid");
		}
	}

	for ( uint8_t i = 0; i < MAX_MESSAGE_CHANNELS; i++ ) {

		ProcessChannelOutOfOrders( m_channels[i] );
	}

	m_timeAtLastReceive = g_masterClock->total.seconds;
}


//----------------------------------------------------------------------------------------------------------------
std::string NetConnection::GetAddressAsString() const {
	return m_remoteAddress.to_string();
}


//----------------------------------------------------------------------------------------------------------------
NetAddress_T NetConnection::GetAddress() const {
	return m_remoteAddress;
}


//----------------------------------------------------------------------------------------------------------------
bool NetConnection::HasTickElapsed() {
	return m_sendTick.CheckAndReset();
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::SetSendRate( float rate ) {
	if (rate > 0.f) {
		m_sendTick.SetTimer( 1.f / rate );
	} else {
		m_sendTick.SetTimer( 0.f );
	}
}


//----------------------------------------------------------------------------------------------------------------
float NetConnection::GetTimeSinceLastReceivedMessage() const {
	return m_timeAtLastReceive;
}


//----------------------------------------------------------------------------------------------------------------
TrackedPacket* NetConnection::AddTrackedPacket( NetPacket* packet, uint8_t ack ) {

	TrackedPacket* trackedPacket = new TrackedPacket();
	uint8_t trackerIndex = ack % MAX_TRACKED_HISTORY_SIZE;
	trackedPacket->SetPacket(packet);
	trackedPacket->SetTimeSent(g_masterClock->total.seconds);

	if (m_trackedPackets[trackerIndex] != nullptr) {
		delete m_trackedPackets[trackerIndex];
	}
	m_trackedPackets[trackerIndex] = trackedPacket;

	return trackedPacket;
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetConnection::GetNextAckToSend() {
	return m_nextSentAck;
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::IncrementNextAckToSend() {
	m_nextSentAck++;
	if (m_nextSentAck == 0xFFFF) {
		m_nextSentAck++;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::ConfirmPacketReceived( uint16_t lastRecvdAck ) {
	
	// If this received ack isn't in the tracked packet slot, something went wrong?
	int trackedPacketSlot = lastRecvdAck % MAX_TRACKED_HISTORY_SIZE;
	if (  m_trackedPackets[ trackedPacketSlot ] != nullptr ) {

		// Check if the tracked packet is valid. If so, calculate rtt, invalidate it and check if it has reliables
		if ( m_trackedPackets[ trackedPacketSlot ]->IsValid() ) {
			m_rtt = Interpolate( m_rtt, g_masterClock->total.seconds - m_trackedPackets[ trackedPacketSlot ]->GetTimeSent(), 0.1f );
			m_trackedPackets[ trackedPacketSlot ]->Invalidate();

			// If it has reliables, iterate through and confirm those reliables.
			uint16_t* reliableIDs = m_trackedPackets[ trackedPacketSlot ]->GetSentReliablesArray();
			for ( int i = 0; i < m_trackedPackets[ trackedPacketSlot ]->GetNumReliablesInPacket(); i++ ) {
				uint16_t id = reliableIDs[ i ];

				// now that we have the actual reliable ID, check it against the unconfirmed
				// reliables and remove it from the list.
				for ( int message = 0; message < m_unconfirmedReliables.size(); message++ ) {
					if ( id == m_unconfirmedReliables[ message ]->GetReliableID() ) {
						delete m_unconfirmedReliables[ message ];
						m_unconfirmedReliables[ message ] = m_unconfirmedReliables[ m_unconfirmedReliables.size() - 1 ];
						m_unconfirmedReliables.pop_back();
						message--;
					}
				}
			} 
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetConnection::GetNextSentAck() {
	return m_nextSentAck;
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetConnection::GetLastRecvdAck() {
	return m_highestRecvdAck;
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetConnection::GetPreviousRecvdAckBitfield() {
	return m_previousRecvdAckBitfield;
}


//----------------------------------------------------------------------------------------------------------------
float NetConnection::GetRTT() {
	return m_rtt;
}


//----------------------------------------------------------------------------------------------------------------
float NetConnection::GetLoss() {
	return m_loss;
}


//----------------------------------------------------------------------------------------------------------------
float NetConnection::GetTimeAtLastReceive() {
	return m_timeAtLastReceive;
}


//----------------------------------------------------------------------------------------------------------------
float NetConnection::GetTimeAtLastSend() {
	return m_timeAtLastSend;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetConnection::GetConnectionIndex() {
	return m_connectionIndex;
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetConnection::GetNumUnconfirmedReliables() {
	return m_unconfirmedReliables.size();
}


//----------------------------------------------------------------------------------------------------------------
uint16_t NetConnection::GetOldestUnconfirmedReliable() {

	if (m_unconfirmedReliables.size() == 0) {
		return 0;
	}

	uint16_t oldest = m_unconfirmedReliables[0]->GetReliableID();
	for ( int i = 0; i < m_unconfirmedReliables.size(); i++ ) {
		if ( CyclicLess( m_unconfirmedReliables[i]->GetReliableID(), oldest ) ) {
			oldest = m_unconfirmedReliables[i]->GetReliableID();
		}
	}
	return oldest;
}


//----------------------------------------------------------------------------------------------------------------
bool NetConnection::CanSendNewReliable() {
	if ( ( m_unconfirmedReliables.size() == 0 ) || ( m_lastSentReliable - GetOldestUnconfirmedReliable() + 1) < RELIABLE_WINDOW) {
		return true;
	}
	else {
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::AddToReceivedReliablesList( uint16_t reliableID ) {

	if ( CyclicLess( m_highestReceivedReliable, reliableID ) ) {
		m_highestReceivedReliable = reliableID;
	}
	m_receivedReliables.push_back( reliableID );

	std::list<uint16_t>::iterator it = m_receivedReliables.begin();

	while (it != m_receivedReliables.end()) {

		if (*it < m_highestReceivedReliable - RELIABLE_WINDOW) {
			it = m_receivedReliables.erase(it);
		} else {
			it++;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::SetSequenceIDOnMessage( NetMessage* msg ) {

	NetCommand const& def = NetSession::GetCommand( msg->GetMessageIndex() );
	msg->SetSequenceID( m_channels[def.channel].GetAndIncrementNextSequenceID() );
}


//----------------------------------------------------------------------------------------------------------------
NetMessageChannel& NetConnection::GetChannelForMessage( NetMessage* msg ) {
	return m_channels[ NetSession::GetCommand( msg->GetMessageIndex() ).channel ];
}


//----------------------------------------------------------------------------------------------------------------
void NetConnection::ProcessChannelOutOfOrders( NetMessageChannel& channel ) {

	std::list<NetMessage*>::iterator it = channel.m_outOfOrderMessages.begin();

	while ( it != channel.m_outOfOrderMessages.end() ) {

		if ( (*it)->GetSequenceID() == channel.m_nextExpectedSequenceID ) {
			NetCommand& command = NetSession::GetCommand( (*it)->GetMessageIndex() );
			command.callback( **it, *this );

			delete *it;
			channel.m_outOfOrderMessages.erase( it );
			it = channel.m_outOfOrderMessages.begin();

			channel.m_nextExpectedSequenceID++;
		}

		else {
			it++;
		}
	}
}