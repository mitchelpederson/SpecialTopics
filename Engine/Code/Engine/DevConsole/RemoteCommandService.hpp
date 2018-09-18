#pragma once

#include "Engine/DevConsole/Command.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Net/TCPSocket.hpp"

#include <vector>

#define GUILDHALL_RCS_SERVICE 29283

class RemoteCommandService {
public:
	RemoteCommandService();
	~RemoteCommandService();

	static void BeginHosting( unsigned int service );
	static void DisconnectAll();
	static void DisconnectClient( unsigned int clientIndex );
	static void ConnectRemoteClient( NetAddress_T const& otherAddr );
	static void ConnectToHost( NetAddress_T const& otherAddr );
	
	static void ProcessFrame();

	static void SendChatToHost( std::string const& message );
	static void SendCommandToClient( Command const& command, bool echo = false, unsigned int clientIndex = 0 );
	static void SendCommandToHost( Command const& message, bool echo = false );
	static void BroadcastCommand( Command const& message, bool echo = false, bool runLocally = false );

	static bool IsConnected();
	static bool IsHost();

	static std::vector<NetAddress_T> GetClientAddresses();

	static std::string RemoveRemoteCommandTokenFromCommand( std::string const& command );

private:

	static void ProcessFrameAsHost();
	static void ProcessFrameAsClient();
	static void ProcessClientAsHost( unsigned int clientIndex );

	static void SendCommandToSocket( TCPSocket& destination, Command const& command, bool echo = true );


	static TCPSocket m_localServerSocket;
	static TCPSocket m_remoteServerSocket;
	static std::vector<TCPSocket*> m_remoteClientConnections;
	static bool m_isHosting;
	static bool m_isClient;
	static bool m_areEchoesEnabled;
};