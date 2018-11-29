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
Stopwatch	NetSession::m_joinTimer = Stopwatch( nullptr );

double NetSession::m_lastReceivedHostTime = 0.0;
double NetSession::m_desiredClientTime = 0.0;
double NetSession::m_currentClientTime = 0.0;
Clock* NetSession::m_sessionClock = nullptr;
float  NetSession::m_deltaTimeDilation = 1.0;

std::vector<NetCommand> NetSession::m_registeredMessages;

NetSession* NetSession::instance = nullptr;

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
	return true;
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
	return true;
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
	
	if ( sender.IsHost() ) {
		double hostTime;
		message.ReadValue<double>( &hostTime );
		
		if ( hostTime > NetSession::instance->GetNetTime() ) {
			NetSession::instance->SetHostTime( hostTime + (sender.GetRTT() / 2.0) );
		}
	}
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::OnCoreDummy( NetMessage& message, NetConnection& sender ) {
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool OnJoinRequest( NetMessage& message, NetConnection& sender ) {
	
	bool shouldAccept = true;
	if ( sender.GetConnectionIndex() == NetSession::instance->GetHostConnection()->GetConnectionIndex() ) {
		shouldAccept = false;
	}
	if ( NetSession::instance->GetNumberOfConnections() >= MAX_CLIENTS ) {
		shouldAccept = false;
	}

	if ( NetSession::instance->IsAddressAlreadyConnected( sender.GetAddress() ) ) {
		shouldAccept = false;
		return true;
	}
	
	char idStr[ 16 ];
	message.ReadString( idStr, 16 );

	NetConnectionInfo_T info;
	info.addr = sender.GetAddress();
	info.id = idStr;
	info.sessionIndex = NetSession::instance->GetNextFreeSessionID();
	NetConnection* client = NetSession::instance->AddConnection( info );

	if ( client == nullptr ) {
		shouldAccept = false;
	}

	if ( shouldAccept ) {
		Logger::PrintTaggedf( "Debug", "Accepting join request from %s", info.addr.to_string().c_str() );

		NetMessage acceptMsg( sender.m_session->GetMessageIndexForName( "join_accept" ) );
		acceptMsg.WriteValue<uint8_t>( info.sessionIndex );
		acceptMsg.WriteValue<double>( NetSession::m_sessionClock->total.hp_seconds );
		client->Send( acceptMsg );

		NetMessage finishMsg( NETMSG_JOIN_FINISHED );
		client->Send( finishMsg );

		client->SetConnectionState( CONNECTION_READY );
	} 
	
	else {
		Logger::PrintTaggedf( "Debug", "Declining join request from %s", info.addr.to_string().c_str() );
		NetMessage denyMsg( sender.m_session->GetMessageIndexForName( "join_deny" ) );
		sender.SendPacketImmediate( sender.m_session->GetSocket(), denyMsg );

		NetSession::instance->DestroyConnection( client );
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool OnJoinAccept( NetMessage& message, NetConnection& sender ) {

	NetSession::instance->SetState( SESSION_READY );

	uint8_t index;
	message.ReadValue<uint8_t>( &index );

	double hostTime;
	message.ReadValue<double>( &hostTime );
	NetSession::instance->InitializeTimesFromHost( hostTime );

	NetConnection* me = NetSession::instance->GetMyConnection();
	me->SetConnectionState( CONNECTION_JOINING );
	NetSession::instance->BindConnection( index, me );

	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool OnJoinDeny( NetMessage& message, NetConnection& sender ) {
	if ( NetSession::instance->GetState() == SESSION_CONNECTING ) {
		NetSession::instance->Disconnect();
		Logger::PrintTaggedf("NetSession", "Received a join deny");
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool OnJoinFinished( NetMessage& message, NetConnection& sender ) {
	if ( NetSession::instance->GetState() == SESSION_JOINING ) {
		NetSession::instance->SetState( SESSION_READY );
		NetSession::instance->GetHostConnection()->SetConnectionState( CONNECTION_READY );
		NetSession::instance->GetMyConnection()->SetConnectionState( CONNECTION_READY );

		return true;
	}
	return false;
}


//----------------------------------------------------------------------------------------------------------------
bool OnUpdateConnState( NetMessage& message, NetConnection& sender ) {

	if ( sender.GetState() == CONNECTION_READY && NetSession::instance->GetState() > SESSION_DISCONNECTED ) {
		sender.SetConnectionState( CONNECTION_DISCONNECTED );
		return true;
	}

	return false;
}


//----------------------------------------------------------------------------------------------------------------
bool OnHangup( NetMessage& message, NetConnection& sender ) {
	NetConnection* senderPtr = NetSession::instance->GetConnection( sender.GetConnectionIndex() );
	senderPtr->SetConnectionState( CONNECTION_DISCONNECTED );

	if ( senderPtr == NetSession::instance->GetHostConnection() ) {
		NetSession::instance->Disconnect();
	}
	return true;
}


//----------------------------------------------------------------------------------------------------------------
// Host/Join Commands
//----------------------------------------------------------------------------------------------------------------
void JoinCommand( std::string const& command ) {
	Command comm( command );

	std::string addrStr;
	std::string id;
	comm.GetNextString( addrStr );
	comm.GetNextString( addrStr );
	comm.GetNextString( id );

	NetAddress_T addr( addrStr.c_str() );

	NetConnectionInfo_T info;
	info.addr = addr;
	info.sessionIndex = 0;
	NetSession::instance->Join( id, info );
}


//----------------------------------------------------------------------------------------------------------------
void HostCommand( std::string const& command ) {
	Command comm( command );
	int port;

	comm.GetFirstToken();
	if ( !comm.GetNextInt( port ) ) {
		port = 10084;
	}
	NetSession::instance->Host( "HOST", port );
}


//----------------------------------------------------------------------------------------------------------------
void DisconnectCommand( std::string const& command ) {
	NetSession::instance->Disconnect();
}



//----------------------------------------------------------------------------------------------------------------
// NetSession code
//----------------------------------------------------------------------------------------------------------------
NetSession::NetSession()
{
	m_sessionClock = new Clock( g_masterClock );
	m_sessionTick.SetClock( m_sessionClock );
	m_sessionTick.SetTimer( 1.f / m_tickRate );
	m_boundConnections.resize( MAX_CLIENTS, nullptr );

	m_joinTimer.SetClock( m_sessionClock );
	m_joinTimer.SetTimer( JOIN_TIMEOUT );

	RegisterCoreMessages();
	
	CommandRegistration::RegisterCommand( "net_sim_lag", SetSimLatencyCommand, "<float> <float> - Sets the min and max latency for the net simulator" );
	CommandRegistration::RegisterCommand( "net_sim_loss", SetSimLossRateCommand, "<float> - Sets the loss rate for the net simulator" );
	CommandRegistration::RegisterCommand( "net_set_session_send_rate", SetTickRateCommand, "<float> - Sets the send rate in Hz");
	CommandRegistration::RegisterCommand( "host", HostCommand, "port - Starts hosting a game net session" );
	CommandRegistration::RegisterCommand( "join", JoinCommand, "ip:port id - Sends a join request to the ip" );
	CommandRegistration::RegisterCommand( "disconnect", DisconnectCommand, " - Sends a join request to the ip" );

	instance = this;
}


//----------------------------------------------------------------------------------------------------------------
NetSession::~NetSession() {
	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		if ( m_boundConnections[i] != nullptr ) {
			m_boundConnections[i] = nullptr;
		}
	}

	std::list< NetConnection* >::iterator it = m_allConnections.begin();
	while ( *it != nullptr ) {
		m_allConnections.erase( it );
		it = m_allConnections.begin();
	}
	
	m_myConnection = nullptr;
	m_hostConnection = nullptr;

	for ( int i = 0; i < m_incomingPackets.size(); i++ ) {
		delete m_incomingPackets[i];
		m_incomingPackets[i] = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::RegisterCoreMessages() {
	RegisterMessage( NETMSG_HEARTBEAT,			"heartbeat",			OnHeartbeat/*,		*/ );
	RegisterMessage( NETMSG_PING,				"ping",					OnPing,				NETMSG_OPTION_CONNECTIONLESS );
	RegisterMessage( NETMSG_PONG,				"pong",					OnPong,				NETMSG_OPTION_CONNECTIONLESS );
	RegisterMessage( NETMSG_JOIN_REQUEST,		"join_request",			OnJoinRequest,		NETMSG_OPTION_CONNECTIONLESS );
	RegisterMessage( NETMSG_JOIN_DENY,			"join_deny",			OnJoinDeny,			NETMSG_OPTION_CONNECTIONLESS );
	RegisterMessage( NETMSG_JOIN_ACCEPT,		"join_accept",			OnJoinAccept,		NETMSG_OPTION_IN_ORDER );
	RegisterMessage( NETMSG_NEW_CONNECTION,		"new_connection",		OnCoreDummy,		NETMSG_OPTION_IN_ORDER );
	RegisterMessage( NETMSG_JOIN_FINISHED,		"join_finished",		OnJoinFinished,		NETMSG_OPTION_IN_ORDER );
	RegisterMessage( NETMSG_UPDATE_CONN_STATE,	"update_conn_state",	OnUpdateConnState,	NETMSG_OPTION_IN_ORDER );
	RegisterMessage( NETMSG_HANGUP,				"hangup",				OnHangup/*,			*/ );
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::Host( std::string const& myID, uint16_t port, uint16_t portRange /* = DEFAULT_PORT_RANGE */ ) {
	
	// We must be currently disconnected to host
	if ( m_state != SESSION_DISCONNECTED ) {
		Logger::PrintTaggedf( "NetSession", "Cannot host - not in a disconnected state!" );
		return;
	}
	
	// Attempt to bind my socket
	if ( !AddBinding( port ) ) {
		Logger::PrintTaggedf( "NetSession", "Cannot host - failed to bind the socket!" );
		return;
	}
	
	NetConnectionInfo_T info;
	info.addr = NetAddress_T::GetLocal( m_socket->GetAddress().port );
	info.sessionIndex = 0;
	info.id = myID;
	NetConnection* me = AddConnection( info );
	BindConnection( 0, me );

	m_myConnection = me;
	m_hostConnection = me;

	me->SetConnectionState( CONNECTION_READY );
	m_state = SESSION_READY;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::Join( std::string const& myID, NetConnectionInfo_T const& hostInfo ) {

	AddBinding( GAME_PORT );

	NetMessage joinRequest( NETMSG_JOIN_REQUEST );
	joinRequest.WriteString( myID.c_str() );

	NetConnection* host = AddConnection( hostInfo );
	host->SendPacketImmediate( m_socket, joinRequest );
	m_hostConnection = host;
	m_hostConnection->SetConnectionState( CONNECTION_CONNECTED );
	
	BindConnection(0, host);

	NetConnectionInfo_T myInfo;
	myInfo.addr = NetAddress_T::GetLocal( m_socket->GetAddress().port );
	myInfo.id = myID;
	myInfo.sessionIndex = 0xFF;
	NetConnection* me = AddConnection( myInfo );
	m_myConnection = me;
	m_myConnection->SetConnectionState( CONNECTION_CONNECTING );

	m_state = SESSION_CONNECTING;

	m_joinTimer.Reset();

}


//----------------------------------------------------------------------------------------------------------------
void NetSession::Disconnect() {

	m_state = SESSION_DISCONNECTED;

	NetMessage disconnectMsg( NETMSG_HANGUP );

	if ( m_hostConnection != m_myConnection ) {
		m_hostConnection->SendPacketImmediate( m_socket, disconnectMsg );
	} else {
		std::list< NetConnection* >::iterator it = m_allConnections.begin();
		while ( it != m_allConnections.end() ) {
			(*it)->SendPacketImmediate( m_socket, disconnectMsg );
			it++;
		}
	}

	m_hostConnection = nullptr;
	m_myConnection = nullptr;

	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		m_boundConnections[i] = nullptr;
	}

	std::list< NetConnection* >::iterator it = m_allConnections.begin();
	while ( it != m_allConnections.end() ) {
		m_allConnections.erase( it );
		it = m_allConnections.begin();
	}

	CloseSocket();
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::Update() {
	
	// Update all connections
	std::list< NetConnection* >::iterator it = m_allConnections.begin();
	while ( it != m_allConnections.end() ) {
		(*it)->Update();
		it++;
	}

	// If any connections marked themselves as disconnected, destroy them.
	it = m_allConnections.begin();
	while ( it != m_allConnections.end() ) {
		if ( (*it)->IsDisconnected() || (*it)->HasTimedOut() ) {
			DestroyConnection( *it );
			it = m_allConnections.begin();
		}
		it++;
	}

	// If we're trying to connect and the join timer times out, disconnect.
	if ( m_state == SESSION_CONNECTING ) {
		if ( m_joinTimer.CheckAndReset() ) {
			Disconnect();
			Logger::PrintTaggedf( "NetSession", "Join timed out" );
		}
	}

	// Update net clock client stuff - if host do nothing
	if ( !AmIHost() ) {
		m_desiredClientTime += m_sessionClock->frame.hp_seconds;

		if ( m_currentClientTime + m_sessionClock->frame.hp_seconds >= m_desiredClientTime ) {
			m_deltaTimeDilation = Max( 1.0f - MAX_NET_TIME_DILATION, m_deltaTimeDilation * 0.995f );
		} else {
			m_deltaTimeDilation = Min( 1.0f + MAX_NET_TIME_DILATION, m_deltaTimeDilation * 1.005f );
		}
		
		m_currentClientTime += m_sessionClock->frame.hp_seconds * m_deltaTimeDilation;
	}
	// If we're host just keep updating host time based on the session clock
	else {
		m_lastReceivedHostTime = m_sessionClock->total.hp_seconds;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessIncoming() {

	if ( m_state != SESSION_DISCONNECTED ) {
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
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessPacket( PacketInLatencySim_T* wrappedPacket ) {
	NetPacket* packet = wrappedPacket->packet;

	NetPacketHeader_T header;
	packet->ReadHeader( header );

	// If it is from a valid connection
	if ( IsValidConnectionIndex( header.connectionIndex ) ) {
		m_boundConnections[header.connectionIndex]->ProcessIncoming( *packet );
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
		p->timestamp = m_sessionClock->total.seconds + m_simLatency.GetRandomInRange();
		p->addr = addr;
		m_incomingPackets.push_back(p);
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessLatencyQueue() {
	for ( int i = 0; i < m_incomingPackets.size(); i++ ) {

		// If the packet's timestamp is older or the same age as our current time,
		// Process it, delete it, and quick remove from the queue
		if ( m_incomingPackets[i]->timestamp <= m_sessionClock->total.seconds ) {
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
		if ( m_boundConnections[ connectionIndex ] != nullptr && m_boundConnections[ connectionIndex ]->GetState() >= CONNECTION_CONNECTED ) {
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ProcessOutgoing() {

	if ( m_state != SESSION_DISCONNECTED ) {

		// Update first so we can send control messages to connections, etc.
		Update(); 

		// Do heartbeats next
		for ( int i = 0; i < MAX_CLIENTS; i++ ) {
			if ( m_boundConnections[i] != nullptr ) {
				m_boundConnections[i]->UpdateHeartbeat();
			}
		}

		// Now do sends based on net ticks. First check if the session tick has come up, if so do the 
		// connection tick check. IF there's no session tick, we still have to see if the connection
		// tick is up.
		if ( m_sessionTick.CheckAndReset() ) {
			std::list< NetConnection* >::iterator it = m_allConnections.begin();
			while ( it != m_allConnections.end() ) {
				if ( *it != nullptr ) {
					if ( (*it)->HasTickElapsed() ) {
						(*it)->SendPacket( m_socket );
					}
				}
				it++;
			}
		} 

		else {
			std::list< NetConnection* >::iterator it = m_allConnections.begin();
			while ( it != m_allConnections.end() ) {
				if ( *it != nullptr ) {
					if ( (*it)->HasTickElapsed() ) {
						(*it)->SendPacket( m_socket );
					}
				}
				it++;
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::AddBinding( unsigned short session ) {

	m_socket = new UDPSocket();
	if ( m_socket->Bind( NetAddress_T::GetLocal( session ), DEFAULT_PORT_RANGE ) ) {
		return true;
	} else {
		m_socket->Close();
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::CloseSocket() {
	if ( m_socket != nullptr ) {
		delete m_socket;
		m_socket = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::RegisterMessage( uint8_t index, std::string const& name, net_message_cb callback, uint16_t flags /* = 0 */, uint8_t channel /* = 0 */ ) {
	NetCommand comm;
	comm.id = index;
	comm.name = name;
	comm.callback = callback;
	comm.flags = flags;
	comm.channel = channel;

	if (m_registeredMessages.size() < index + 1) {
		m_registeredMessages.resize( index + 1 );
	}
	m_registeredMessages[index] = comm;
}


//----------------------------------------------------------------------------------------------------------------
NetConnection* NetSession::AddConnection( NetConnectionInfo_T const& info ) {
	if ( m_allConnections.size() == MAX_CLIENTS ) {
		return nullptr;
	}

	NetConnection* conn = new NetConnection( this, info );
	
	if ( conn != nullptr ) {
		m_allConnections.push_back( conn );
	}

	if ( conn->GetConnectionIndex() < MAX_CLIENTS && m_boundConnections[ conn->GetConnectionIndex() ] == nullptr ) {
		BindConnection( info.sessionIndex, conn );
	}

	return conn;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::DestroyConnection( NetConnection* conn ) {
	// If it's my connection or the host connection, empty the convenience pointers
	if ( m_myConnection == conn ) {
		m_myConnection = nullptr;
	}
	if ( m_hostConnection == conn ) {
		m_hostConnection = nullptr;
	}

	// If it's a bound connection, unbind it without destroying the connection yet
	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		if ( m_boundConnections[i] == conn ) {
			m_boundConnections[i] = nullptr;
			break;
		}
	}

	// Remove it from all connections
	std::list< NetConnection* >::iterator it = m_allConnections.begin();
	while ( it != m_allConnections.end() ) {
		if ( *it == conn ) {
			m_allConnections.erase( it );
			break;
		}
		it++;
	}

	// Now destroy the connection
	delete conn;
}


//----------------------------------------------------------------------------------------------------------------
NetConnection* NetSession::GetConnection( unsigned int index ) {
	if ( index >= MAX_CLIENTS || !IsValidConnectionIndex( index ) ) {
		return nullptr;
	}
	return m_boundConnections[index];
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::BindConnection( uint8_t index, NetConnection* conn ) {
	if ( index < MAX_CLIENTS && m_boundConnections[index] == nullptr ) {
		m_boundConnections[ index ] = conn;
		conn->SetConnectionIndex( index );
	}
	
}


//----------------------------------------------------------------------------------------------------------------
net_message_cb NetSession::GetCallbackForMessage( uint8_t index ) {
	return m_registeredMessages[index].callback;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetSession::GetMessageIndexForName( std::string const& name ) {
	for ( int i = 0; i < m_registeredMessages.size(); i++ ) {
		if ( m_registeredMessages[i].name.compare(name) == 0 ) {
			return (uint8_t) i;
		}
	}
	return 0xFF;
}


//----------------------------------------------------------------------------------------------------------------
unsigned int NetSession::GetNumberOfUsers() const {
	int userCount = 0;
	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		if (m_boundConnections[i] != nullptr) {
			userCount++;
		}
	}
	return userCount;
}


//----------------------------------------------------------------------------------------------------------------
unsigned int NetSession::GetNumberOfConnections() const {
	return m_allConnections.size();
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetSession::GetMyConnectionIndex() const {
	if (m_myConnection != nullptr) {
		return m_myConnection->GetConnectionIndex();
	} else {
		return 0xFF;
	}
}


//----------------------------------------------------------------------------------------------------------------
NetAddress_T NetSession::GetMyAddress() const {
	return m_socket->GetAddress();
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
	if ( IsValidConnectionIndex( (uint16_t) index ) ) {
		m_boundConnections[ index ]->SetSendRate( rate );
	}
}


//----------------------------------------------------------------------------------------------------------------
float NetSession::GetTimeSinceLastMessageOnConnection( unsigned int connIndex ) {
	if ( IsValidConnectionIndex( connIndex ) ) {
		return m_sessionClock->total.seconds - m_boundConnections[connIndex]->GetTimeSinceLastReceivedMessage();
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
	return m_myConnection;
}


//----------------------------------------------------------------------------------------------------------------
NetCommand NetSession::GetCommand( uint8_t index ) {
	return m_registeredMessages[index];
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetError( eNetSessionError error, std::string str /* = "" */ ) {
	m_error = error;
	m_errorString = str;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::ClearError() {
	m_error = SESSION_OK;
	m_errorString = "";
}


//----------------------------------------------------------------------------------------------------------------
eNetSessionError NetSession::GetLastNetError() {
	eNetSessionError err = m_error;
	m_error = SESSION_OK;
	return err;
}


//----------------------------------------------------------------------------------------------------------------
NetConnection* NetSession::GetHostConnection() const {
	return m_hostConnection;
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::IsConnected() const {
	return m_state >= SESSION_JOINING;
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::IsBound() const {
	return m_state >= SESSION_BOUND;
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::IsDisconnected() const {
	return m_state == SESSION_DISCONNECTED;
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::IsReady() const {
	return m_state == SESSION_READY;
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::IsAddressAlreadyConnected( NetAddress_T const& addr ) {
	
	std::list< NetConnection* >::iterator it = m_allConnections.begin();

	while ( it != m_allConnections.end() ) {
		if ( (*it)->GetAddress() == addr ) {
			return true;
		}
		it++;
	}

	return false;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t NetSession::GetNextFreeSessionID() const {
	for ( uint8_t i = 1; i < m_boundConnections.size(); i++ ) {
		if ( m_boundConnections[i] == nullptr) {
			return i;
		}
	}
	return 0xFF;
}


//----------------------------------------------------------------------------------------------------------------
eNetSessionState NetSession::GetState() const {
	return m_state;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetState( eNetSessionState state ) {
	m_state = state;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::InitializeTimesFromHost( double hostTime ) {
	SetHostTime( hostTime );
	m_currentClientTime = hostTime;
}


//----------------------------------------------------------------------------------------------------------------
void NetSession::SetHostTime( double hostTime ) {
	m_lastReceivedHostTime = hostTime;
	if ( !AmIHost() ) {
		m_desiredClientTime = m_lastReceivedHostTime;
	}
}


//----------------------------------------------------------------------------------------------------------------
double NetSession::GetNetTime() {
	if ( AmIHost() ) {
		return m_lastReceivedHostTime;
	} else {
		return m_currentClientTime;
	}
}


//----------------------------------------------------------------------------------------------------------------
double NetSession::GetHostTime() {
	return m_lastReceivedHostTime;
}


//----------------------------------------------------------------------------------------------------------------
bool NetSession::AmIHost() const {
	return m_myConnection == m_hostConnection;
}