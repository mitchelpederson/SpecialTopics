#include "Engine/DevConsole/RemoteCommandService.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

TCPSocket RemoteCommandService::m_localServerSocket;
TCPSocket RemoteCommandService::m_remoteServerSocket;
std::vector<TCPSocket*> RemoteCommandService::m_remoteClientConnections;
bool RemoteCommandService::m_isHosting = false;
bool RemoteCommandService::m_isClient = false;


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
RemoteCommandService::RemoteCommandService() {
	CommandRegistration::RegisterCommand("rc_host", BeginHostingCommand, "Disconnects any rcs connections and sets up as a host");
	CommandRegistration::RegisterCommand("rc_join", JoinHostCommand, "Disconnects any rcs connections and tries to connect to a new host");
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
	while ( newClient != nullptr ) {
		m_remoteClientConnections.push_back( newClient );
		newClient = m_localServerSocket.Accept();
	}

	// Check for new messages from all clients
	for ( unsigned int index = 0; index < m_remoteClientConnections.size(); index++ ) {
		char* buffer[256];
		m_remoteClientConnections[index]->Receive( buffer, 255 );
		buffer[255] = '\0';
		DevConsole::Printf("%s", buffer);
	}
}


//----------------------------------------------------------------------------------------------------------------
void RemoteCommandService::ProcessFrameAsClient() {

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
		char const* str = "Hi";
		m_remoteServerSocket.Send(str, 3);
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