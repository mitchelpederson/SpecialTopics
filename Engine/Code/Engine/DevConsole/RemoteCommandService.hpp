#pragma once

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


private:

	static void ProcessFrameAsHost();
	static void ProcessFrameAsClient();

	static TCPSocket m_localServerSocket;
	static TCPSocket m_remoteServerSocket;
	static std::vector<TCPSocket*> m_remoteClientConnections;
	static bool m_isHosting;
	static bool m_isClient;
};