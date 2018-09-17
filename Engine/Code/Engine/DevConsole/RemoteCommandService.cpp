#include "Engine/DevConsole/RemoteCommandService.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

TCPSocket RemoteCommandService::m_localServerSocket;
TCPSocket RemoteCommandService::m_remoteServerSocket;
std::vector<TCPSocket*> RemoteCommandService::m_remoteClientConnections;
bool RemoteCommandService::m_isHosting = false;
bool RemoteCommandService::m_isClient = false;
bool RemoteCommandService::m_areEchoesEnabled = true;


//----------------------------------------------------------------------------------------------------------------
void BeginHostingCommand( std::string const& command ) {
	RemoteCommandService::BeginHosting( GUILDHALL_RCS_SERVICE );
}


//----------------------------------------------------------------------------------------------------------------
void JoinHostCommand( std::string const& command ) {
	std::vector<std::string> tokens = SplitString( command, ' ' );
	NetAddress_T addr( tokens[1].c_str() );

	DevConsole::Printf( "Trying to connect to %s", tokens[1].c_str() );

	RemoteCommandService::ConnectToHost( NetAddress_T( tokens[1].c_str() ) );
}


//----------------------------------------------------------------------------------------------------------------
void RCSSendChatMessage( std::string const& command ) {
	std::vector<std::string> tokens = SplitString( command, ' ' );
	RemoteCommandService::SendChatToHost( tokens[1] );
	
}


//----------------------------------------------------------------------------------------------------------------
void RCSSendCommandToClient( std::string const& command ) {

	// If we aren't hosting, we can't do this.
	if ( !RemoteCommandService::IsHost() ) {
		DevConsole::Printf( "You can't tell a client to do something if you're not a host" );
		return;
	}

	Command comm( command );

	std::string indexStr = "";
	comm.GetNextString( indexStr ); // skip the rc
	comm.GetNextString( indexStr ); // this will either be a command or idx=*

	int index = 0;
	int substrOffset = 3;
	if ( comm.GetNextInt( index ) ) {
		if (index < 10) {
			substrOffset += 2;
		} else {
			substrOffset += 3;
		}
	}

	std::string sentCommStr = command.substr( substrOffset );
	Command sentComm( sentCommStr );
	RemoteCommandService::SendCommandToClient( sentComm, false, index );

}


//----------------------------------------------------------------------------------------------------------------
void RCSSendCommandToHost( std::string const& command ) {
	// If we aren't hosting, we can't do this.
	if ( !RemoteCommandService::IsConnected() ) {
		DevConsole::Printf( "You can't tell a host to do something if you're not a client" );
		return;
	}

	Command comm( command );

	int substrOffset = 3;

	std::string sentCommStr = command.substr( substrOffset );
	Command sentComm( sentCommStr );
	RemoteCommandService::SendCommandToHost( sentComm, false );
}


//----------------------------------------------------------------------------------------------------------------
void RCSSendCommand( std::string const& command ) {
	if ( RemoteCommandService::IsHost() ) {
		RCSSendCommandToClient( command );
	} 
	else if ( RemoteCommandService::IsConnected() ){
		RCSSendCommandToHost( command );
	}
}


//----------------------------------------------------------------------------------------------------------------
RemoteCommandService::RemoteCommandService() {
	CommandRegistration::RegisterCommand("rc_host", BeginHostingCommand, "Disconnects any rcs connections and sets up as a host");
	CommandRegistration::RegisterCommand("rc_join", JoinHostCommand, "Disconnects any rcs connections and tries to connect to a new host");
	CommandRegistration::RegisterCommand("rc_chat", RCSSendChatMessage, "Sends a message to the host");
	CommandRegistration::RegisterCommand("rc", RCSSendCommand, "Sends a command from a host to a client");
}


//----------------------------------------------------------------------------------------------------------------
RemoteCommandService::~RemoteCommandService() {

}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessFrame() {
	if ( m_isHosting ) {
		ProcessFrameAsHost();
	}

	else if ( m_isClient ) {
		ProcessFrameAsClient();
	}
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessFrameAsHost() {

	// Check for new clients
	TCPSocket* newClient = m_localServerSocket.Accept();
	if ( newClient != nullptr && !newClient->HasFatalError() ) {
		m_remoteClientConnections.push_back( newClient );
		DevConsole::Printf("Someone connected!");
		//newClient = m_localServerSocket.Accept();
	}
	else {
		if (newClient->HasFatalError()) {
			DevConsole::Printf("Failed to accept socket");
		}
	}

	// Check for new messages from all clients
	for ( unsigned int index = 0; index < m_remoteClientConnections.size(); index++ ) {
		char buffer[256];
		int receiveResult = m_remoteClientConnections[index]->Receive( buffer, 255 );
		if ( receiveResult > 0) {
			if (::WSAGetLastError() == 0) {

				BytePacker packedMessage( receiveResult, (byte_t*) buffer, BIG_ENDIAN );
				uint16_t messageSize;
				uint8_t shouldEcho;
				char message[256] = "";

				packedMessage.ReadValue<uint16_t>(&messageSize);
				packedMessage.ReadValue<uint8_t>(&shouldEcho);
				size_t messageStringSize = packedMessage.ReadString(message, 255);
				message[messageStringSize + 1] = '\0';

				Command command( message );
				CommandRegistration::RunCommand( command );

				Logger::PrintTaggedf("RCS", "size: %u; should echo: %u; message: %s", messageSize, shouldEcho, message);

			}
		} else if ( receiveResult == 0 ) {
			DisconnectClient( index );
		} 
	}
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessFrameAsClient() {
	byte_t buffer[0xFFFF];
	int bytesReceived = m_remoteServerSocket.Receive( (void*) buffer, 0xFFFF );

	if ( bytesReceived > 0) {
		if (::WSAGetLastError() == 0) { 

			BytePacker packedMessage( bytesReceived, (byte_t*) buffer, BIG_ENDIAN );
			uint16_t messageSize;
			uint8_t shouldEcho;
			char message[256] = "";
			packedMessage.ReadValue<uint16_t>(&messageSize);
			packedMessage.ReadValue<uint8_t>(&shouldEcho);
			size_t messageStringSize = packedMessage.ReadString(message, 255);
			message[messageStringSize + 1] = '\0';

			Command command( message );
			CommandRegistration::RunCommand( command );

			Logger::PrintTaggedf("RCS", "size: %u; should echo: %u; message: %s", messageSize, shouldEcho, message);
		}
	} else if ( bytesReceived == 0 ) {
		DisconnectAll();
	}
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessClientAsHost( unsigned int clientIndex ) {
	byte_t buffer[0xFFFF];
	TCPSocket* client = m_remoteClientConnections[ clientIndex ];
	int bytesReceived = client->Receive( (void*) buffer, 0xFFFF );

	if ( bytesReceived > 0) {
		if (::WSAGetLastError() == 0) { 

			BytePacker packedMessage( bytesReceived, (byte_t*) buffer, BIG_ENDIAN );
			uint16_t messageSize;
			uint8_t shouldEcho;
			char message[256] = "";
			packedMessage.ReadValue<uint16_t>(&messageSize);
			packedMessage.ReadValue<uint8_t>(&shouldEcho);
			size_t messageStringSize = packedMessage.ReadString(message, 255);
			message[messageStringSize + 1] = '\0';

			Command command( message );
			CommandRegistration::RunCommand( command );

			Logger::PrintTaggedf("RCS", "size: %u; should echo: %u; message: %s", messageSize, shouldEcho, message);
		}
	} else if ( bytesReceived == 0 ) {
		DisconnectClient( clientIndex );
	}
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::SendCommandToClient( Command const& command, bool echo /* = true */, unsigned int clientIndex /* = 0 */ ) {
	if ( clientIndex < m_remoteClientConnections.size() ) {
		SendCommandToSocket( *m_remoteClientConnections[clientIndex], command, echo );
	}

	else {
		DevConsole::Printf( "No client connected at index %u", clientIndex );
	}
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::SendCommandToHost( Command const& message, bool echo /* = true */ ) {

	SendCommandToSocket( m_remoteServerSocket, message, echo );
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::BeginHosting( unsigned int service ) {
	DisconnectAll();

	if ( !m_localServerSocket.Listen( service, 32 ) ) {
		ERROR_RECOVERABLE( "Could not start RCS on service %u", service );
	}
	m_isHosting = true;
	m_isClient = false;
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::ConnectToHost( NetAddress_T const& otherAddr ) {
	DisconnectAll();

	if ( m_remoteServerSocket.Connect( otherAddr ) ) {
		DevConsole::Printf("Connected to host");
		char const* str = "Hi";
		m_remoteServerSocket.Send(str, 3);
		m_isClient = true;
	}
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::DisconnectAll() {
	if (!m_localServerSocket.IsClosed()) {
		m_localServerSocket.Close();
	}

	if (!m_remoteServerSocket.IsClosed()) {
		m_remoteServerSocket.Close();
	}

	for ( int i = m_remoteClientConnections.size() - 1; i >= 0; i-- ) {
		if ( !m_remoteClientConnections[i]->IsClosed()) {
			m_remoteClientConnections[i]->Close();
		}
	}

	m_isHosting = false;
	m_isClient = false;
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::DisconnectClient( unsigned int clientIndex ) {
	m_remoteClientConnections[ clientIndex ]->Close();
	delete m_remoteClientConnections[ clientIndex ];

	m_remoteClientConnections[ clientIndex ] = m_remoteClientConnections.back();
	m_remoteClientConnections.pop_back();
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::SendChatToHost( std::string const& message ) {
	if (m_isClient) {
		m_remoteServerSocket.Send(message.c_str(), message.size());	
	}
}


//----------------------------------------------------------------------------------------------------------------
bool RemoteCommandService::IsConnected() {
	return m_isClient;
}


//----------------------------------------------------------------------------------------------------------------
bool RemoteCommandService::IsHost() {
	return m_isHosting;
}


//----------------------------------------------------------------------------------------------------------------
std::vector<NetAddress_T> RemoteCommandService::GetClientAddresses() {
	std::vector<NetAddress_T> addrs;
	for ( unsigned int index = 0; index < m_remoteClientConnections.size(); index++ ) {
		addrs.push_back( m_remoteClientConnections[index]->address );
	}
	return addrs;
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::SendCommandToSocket( TCPSocket& destination, Command const& command, bool shouldEcho /* = true */ ) {

	BytePacker packer( BIG_ENDIAN );

	packer.WriteValue<bool>(shouldEcho);
	packer.WriteString( command.GetString().c_str() );

	size_t packedLength = packer.GetWrittenByteCount() + 1;
	if (packedLength > 0xFFFF) {
		ERROR_RECOVERABLE( "Tried to send a command that was too long!" );
		return;
	}

	uint16_t shortLength = (uint16_t) packedLength;
	ToEndianness( 2, &shortLength, BIG_ENDIAN );

	destination.Send( &shortLength, 2 );
	destination.Send( packer.GetBuffer(), packedLength );
}