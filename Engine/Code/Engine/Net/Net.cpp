#include "Engine/Core/WindowsCommon.hpp"
#include <stdint.h>

#include "Engine/Net/Net.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/TCPSocket.hpp"

#include "Engine/Async/Threads.hpp"

#include "Engine/DevConsole/Command.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//----------------------------------------------------------------------------------------------------------------
void TestHostThread( void* data ) {
	unsigned int* portAddr = (unsigned int*) data;
	TestHost( *portAddr );
}


//----------------------------------------------------------------------------------------------------------------
void TestHostCommand( const std::string& command ) {
	std::vector<std::string> tokens = SplitString( command, ' ' );

	unsigned int port = 12345;
	if ( tokens.size() > 1 ) {
		port = (unsigned int) stoi( tokens[1] );
	}

	CreateAndDetachThread( "TestHostThread()", TestHostThread, (void*) &port );
}


//----------------------------------------------------------------------------------------------------------------
void TestConnectCommand( const std::string& command ) {
	std::vector<std::string> tokens = SplitString( command, ' ' );
	NetAddress_T addr( tokens[1].c_str() );

	DevConsole::Printf("Trying to connect to %s", tokens[1].c_str() );

	if (tokens.size() > 2) {
		TestConnection( addr, tokens[2] );
	}
	else {
		TestConnection( addr, "PING" );
	}
}


//----------------------------------------------------------------------------------------------------------------
bool Net::Startup() {

	WORD version = MAKEWORD( 2, 2 );

	WSADATA data;
	int32_t error = ::WSAStartup( version, &data );

	GUARANTEE_OR_DIE( error == 0, "Error in WSAStartup" );

	CommandRegistration::RegisterCommand( "net_print_local_ip", GetAddressExample, "Prints this machine's local IP" );
	CommandRegistration::RegisterCommand( "a01_test_server", TestHostCommand, "Starts up a new thread with a01a's test server" );
	CommandRegistration::RegisterCommand( "a01_test_connect", TestConnectCommand, "Tries to connect to an a01a test server to send a message" );

	return (error == 0);
}


//----------------------------------------------------------------------------------------------------------------
void Net::Shutdown() {
	::WSACleanup();
}


//----------------------------------------------------------------------------------------------------------------
void TestConnection( NetAddress_T const& address, std::string const& message ) {

	TCPSocket toConnectTo;

	if (!toConnectTo.Connect( address ) ) {
		DevConsole::Printf("Failed to connect!!!");
		return;
	}
	
	if ( toConnectTo.IsClosed() ) {
		ERROR_AND_DIE("Could not create socket");
		return;
	}

	DevConsole::Printf("Connected.");

	// Send our message and receive the message back from the server
	char payload[256];
	toConnectTo.Send( message.c_str(), message.size() );

	size_t received = toConnectTo.Receive( payload, 255 );
	if (received > 0) {
		payload[255] = '\0';
		DevConsole::Printf("Received: %s", payload);
	}
	

	toConnectTo.Close();
}


//----------------------------------------------------------------------------------------------------------------
void TestHost( int port ) {

	TCPSocket hostSocket;
	if ( hostSocket.Listen( (uint16_t) port, 32 ) ) {
		DevConsole::Printf("Created test listen socket");
	} 

	// Loop around and accept some messages!
	bool isRunning = true;
	char* buffer[256];
	while ( isRunning ) {
		TCPSocket* theirSocket = hostSocket.Accept();

		if ( theirSocket != nullptr ) {

			int receivedBytes = theirSocket->Receive(buffer, 255);

			if ( receivedBytes > 0  ) {
				buffer[receivedBytes] = NULL;
				DevConsole::Printf( "Received: %s", buffer);

				theirSocket->Send( "PONG!", 20);
			}

			theirSocket->Close();
			delete theirSocket;
			theirSocket = nullptr;
		}
	}

	hostSocket.Close();
}
