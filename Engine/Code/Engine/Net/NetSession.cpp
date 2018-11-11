#pragma once
#include "Engine/Net/NetSession.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/DevConsole/Command.hpp"

#include "Engine/Core/EngineCommon.hpp"


typedef bool (*net_message_cb)( NetMessage& message, NetConnection& sender );

FloatRange  NetSession::m_simLatency = FloatRange( 0.0f, 0.0f );
float		NetSession::m_simLossRate = 0.f;
float		NetSession::m_tickRate = 20.f;
Stopwatch	NetSession::m_sessionTick = Stopwatch( nullptr );

std::vector<NetCommand> NetSession::m_registeredMessages;


//----------------------------------------------------------------------------------------------------------------
// Test commands for a02
//----------------------------------------------------------------------------------------------------------------
bool OnPing( NetMessage& message, NetConnection& sender ) {

	char buffer[255];
	size_t strSize = message.ReadString( buffer, 255 );
	buffer[strSize] = '\0';

	std::string senderAddress = sender.GetAddressAsString();

	DevConsole::Printf( "Ping from %s: %s", senderAddress.c_str(), buffer );
	
	NetMessage pong( sender.m_session->GetMessageIndexForName( "pong" ) );
	sender.SendPacketImmediate( sender.m_session->GetSocket(), pong );

	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool OnPong( NetMessage& message, NetConnection& sender ) {
	std::string senderAddress = sender.GetAddressAsString();
	DevConsole::Printf( "Pong from %s", senderAddress.c_str() );
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool OnAdd( NetMessage& message, NetConnection& sender ) {
	float a;
	float b;
	float sum;

	if ( !message.ReadValue<float>( &a ) || !message.ReadValue<float>( &b ) ) {
		return false;
	}

	sum = a + b;

	NetMessage response( sender.m_session->GetMessageIndexForName( "add_response" ) );
	response.WriteValue<float>( a );
	response.WriteValue<float>( b );
	response.WriteValue<float>( sum );
	sender.SendPacketImmediate( sender.m_session->GetSocket(), response );

	DevConsole::Printf( "Add: %f + %f = %f", a, b, sum );
}


//----------------------------------------------------------------------------------------------------------------
bool OnAddResponse( NetMessage& message, NetConnection& sender ) {
	float a;
	float b;
	float sum;

	if ( !message.ReadValue<float>( &a ) || !message.ReadValue<float>( &b ) || !message.ReadValue<float>( &sum ) ) {
		return false;
	}

	DevConsole::Printf( "[%s] Add: %f + %f = %f", sender.GetAddressAsString().c_str(), a, b, sum );
}



//----------------------------------------------------------------------------------------------------------------
// Sim Control Commands
//----------------------------------------------------------------------------------------------------------------
void NetSession::SetSimLatencyCommand( std::string const& command ) {
	Command comm( command );
	float latencyMin;
	float latencyMax;

	comm.GetFirstToken();
	if ( !comm.GetNextFloat( latencyMin ) ) {
		return;
	}
	if ( !comm.GetNextFloat( latencyMax ) ) {
		return;
	}

	m_simLatency = FloatRange( latencyMin, latencyMax );
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetSimLossRateCommand( std::string const& command ) {
	Command comm( command );
	float loss;

	comm.GetFirstToken();
	if ( !comm.GetNextFloat( loss ) ) {
		return;
	}

	m_simLossRate = loss;
}


//----------------------------------------------------------------------------------------------------------------
// Control message callbacks
//----------------------------------------------------------------------------------------------------------------
bool NetSession::OnHeartbeat( NetMessage& message, NetConnection& sender ) {
	return true;
}


//----------------------------------------------------------------------------------------------------------------
// NetSession code
//----------------------------------------------------------------------------------------------------------------
NetSession::NetSession() 
{
	m_sessionTick.SetClock( g_masterClock );
	m_sessionTick.SetTimer( 1.f / m_tickRate );
	m_connections.resize( MAX_CLIENTS, nullptr );

	RegisterCoreMessages();
	
	CommandRegistration::RegisterCommand( "net_sim_lag", SetSimLatencyCommand, "<float> <float> - Sets the min and max latency for the net simulator" );
	CommandRegistration::RegisterCommand( "net_sim_loss", SetSimLossRateCommand, "<float> - Sets the loss rate for the net simulator" );
	CommandRegistration::RegisterCommand( "net_set_session_send_rate", SetTickRateCommand, "<float> - Sets the send rate in Hz");
}


//----------------------------------------------------------------------------------------------------------------
NetSession::~NetSession() {
	for ( int i = 0; i < m_connections.size(); i++ ) {
		if ( m_connections[i] != nullptr ) {
			delete m_connections[i];
			m_connections[i] = nullptr;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::RegisterCoreMessages() {
	RegisterMessage( NETMSG_HEARTBEAT,	"heartbeat",	OnHeartbeat/*,	*/ );
	RegisterMessage( NETMSG_PING,		"ping",			OnPing,			NETMSG_OPTION_CONNECTIONLESS );
	RegisterMessage( NETMSG_PONG,		"pong",			OnPong,			NETMSG_OPTION_CONNECTIONLESS );
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessIncoming() {

	char buffer[ MTU ];

	NetAddress_T from_addr; 
	size_t read = m_socket->ReceiveFrom( from_addr, buffer, MTU ); 
	while ( read > 0U ) {

		NetPacket* packet = new NetPacket( buffer, read );
		AddPacketToLatencyQueue( packet, from_addr );

		read = m_socket->ReceiveFrom( from_addr, buffer, MTU ); 
	}

	ProcessLatencyQueue();
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessPacket( PacketInLatencySim_T* wrappedPacket ) {
	NetPacket* packet = wrappedPacket->packet;

	NetPacketHeader_T header;
	packet->ReadHeader( header );

	

	// If it is from a valid connection
	if ( IsValidConnectionIndex( header.connectionIndex ) ) {
		m_connections[header.connectionIndex]->ProcessIncoming( *packet );
	}

	// If the packet doesn't specify a connection it came from
	else {

		for ( int i = 0; i < header.messageCount; i++ ) {
			NetMessage* message = packet->ReadMessage( this );
			NetCommand netCommand = GetCommand( message->GetMessageIndex() );

			if (!netCommand.RequiresConnection()) {
				NetConnection tempConnection( this, 0xFF, wrappedPacket->addr );
				netCommand.callback( *message, tempConnection );
			}
			else {
				DevConsole::Printf("Received a message that requires a connection without one");
			}
			
			delete message;
			message = nullptr;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::AddPacketToLatencyQueue( NetPacket* packet, NetAddress_T addr ) {
	if ( GetRandomFloatZeroToOne() > m_simLossRate ) {
		PacketInLatencySim_T* p = new PacketInLatencySim_T();
		p->packet = packet;
		p->timestamp = g_masterClock->total.seconds + m_simLatency.GetRandomInRange();
		p->addr = addr;
		m_incomingPackets.push_back(p);
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessLatencyQueue() {
	for ( int i = 0; i < m_incomingPackets.size(); i++ ) {

		// If the packet's timestamp is older or the same age as our current time,
		// Process it, delete it, and quick remove from the queue
		if ( m_incomingPackets[i]->timestamp <= g_masterClock->total.seconds ) {
			ProcessPacket( m_incomingPackets[i] );
			delete m_incomingPackets[i];
			m_incomingPackets[i] = m_incomingPackets[ m_incomingPackets.size()-1 ];
			m_incomingPackets.pop_back();
			i--;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::IsValidConnectionIndex( uint8_t connectionIndex ) {
	
	if ( connectionIndex < MAX_CLIENTS ) {
		if ( m_connections[ connectionIndex ] != nullptr ) {
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessOutgoing() {

	// Do heartbeats first
	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		if ( m_connections[i] != nullptr ) {
			m_connections[i]->UpdateHeartbeat();
		}
	}

	// Now do sends based on net ticks. First check if the session tick has come up, if so do the 
	// connection tick check. IF there's no session tick, we still have to see if the connection
	// tick is up.
	if ( m_sessionTick.CheckAndReset() ) {
		for ( int i = 0; i < MAX_CLIENTS; i++ ) {
			if (m_connections[i] != nullptr) {
				if ( m_connections[i]->HasTickElapsed() ) {
					m_connections[i]->SendPacket( m_socket );
				}
			}
		}
	} else {
		for ( int i = 0; i < MAX_CLIENTS; i++ ) {
			if (m_connections[i] != nullptr) {
				if ( m_connections[i]->HasTickElapsed() ) {
					m_connections[i]->SendPacket( m_socket );
				}
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::AddBinding( unsigned short session ) {

	m_socket = new UDPSocket();
	if ( m_socket->Bind( NetAddress_T::GetLocal( session ), 16 ) ) {
		return true;
	} else {
		m_socket->Close();
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::RegisterMessage( uint8_t index, std::string const& name, net_message_cb callback, uint16_t flags /* = 0 */ ) {
	NetCommand comm;
	comm.id = index;
	comm.name = name;
	comm.callback = callback;
	comm.flags = flags;

	if (m_registeredMessages.size() < index + 1) {
		m_registeredMessages.resize( index + 1 );
	}
	m_registeredMessages[index] = comm;
}


//----------------------------------------------------------------------------------------------------------------
NetConnection* NetSession::AddConnection( unsigned int index, NetAddress_T address ) {
	if ( index >= MAX_CLIENTS ) {
		return nullptr;
	}

	if ( m_connections[index] != nullptr ) {
		return nullptr;
	}

	NetConnection* conn = new NetConnection( this, index, address );
	m_connections[index] = conn;

	if ( conn->GetAddress() == m_socket->GetAddress() ) {
		m_myConnection = index;
	}

	return conn;
}


//----------------------------------------------------------------------------------------------------------------
NetConnection* NetSession::GetConnection( unsigned int index ) {
	if ( index >= MAX_CLIENTS ) {
		return nullptr;
	}
	return m_connections[index];
}


//----------------------------------------------------------------------------------------------------------------
net_message_cb NetSession::GetCallbackForMessage( uint8_t index ) {
	return m_registeredMessages[index].callback;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetSession::GetMessageIndexForName( std::string const& name ) {
	for ( int i = 0; i < m_registeredMessages.size(); i++ ) {
		if ( m_registeredMessages[i].name.compare(name) == 0 ) {
			return i;
		}
	}
	return 0xFF;
}


//----------------------------------------------------------------------------------------------------------------
unsigned int NetSession::GetNumberOfUsers() const {
	int userCount = 0;
	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		if (m_connections[i] != nullptr) {
			userCount++;
		}
	}
	return userCount;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetSession::GetMyConnectionIndex() const {
	return m_myConnection;
}


//----------------------------------------------------------------------------------------------------------------
NetAddress_T NetSession::GetMyAddress() const {
	return m_socket->GetAddress();
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetNetIndex( uint8_t index ) {
	m_myConnection = index;
}


//----------------------------------------------------------------------------------------------------------------
UDPSocket* NetSession::GetSocket() const {
	return m_socket;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetSimLatency( float min, float max ) {
	m_simLatency = FloatRange(min, max);
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetSimLossRate( float rate ) {
	m_simLossRate = rate;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetTickRateCommand( std::string const& command ) {
	Command comm( command );
	comm.GetFirstToken();

	float tick;

	if ( !comm.GetNextFloat( tick ) ) {
		return;
	}

	m_tickRate = tick;
	m_sessionTick.SetTimer( 1.f / tick );
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetConnectionTickRate( int index, float rate ) {
	if ( IsValidConnectionIndex( index ) ) {
		m_connections[ index ]->SetSendRate( rate );
	}
}


//----------------------------------------------------------------------------------------------------------------
float NetSession::GetTimeSinceLastMessageOnConnection( unsigned int connIndex ) {
	if ( IsValidConnectionIndex( connIndex ) ) {
		return g_masterClock->total.seconds - m_connections[connIndex]->GetTimeSinceLastReceivedMessage();
	}
	return 0.f;
}


//----------------------------------------------------------------------------------------------------------------
float NetSession::GetSimLossRate() const {
	return m_simLossRate;
}


//----------------------------------------------------------------------------------------------------------------
FloatRange NetSession::GetSimLatency() const {
	return m_simLatency;
}


//----------------------------------------------------------------------------------------------------------------
NetConnection* NetSession::GetMyConnection() const {
	return m_connections[m_myConnection];
}


NetCommand NetSession::GetCommand( uint8_t index ) {
	return m_registeredMessages[index];
}