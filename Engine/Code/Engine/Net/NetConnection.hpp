//----------------------------------------------------------------------------------------------------------------
// NetConnection.hpp
// Mitchel Pederson
//
// A reliable UDP connection between me and another UDP service
//
// We should probably queue the sends here rather than on NetSession
// 
//----------------------------------------------------------------------------------------------------------------

#pragma once 
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetMessage.hpp"

class NetConnection {

public:
	NetConnection( UDPSocket* sock );
	~NetConnection();

	int SendTo( NetMessage const& message );
	int ReceiveFrom( NetMessage const& message );

	void BindLocal( unsigned short service );
	void Connect( UDPSocket* sock );
	void Close();

	float GetTimeOfLastReceivedMessage() const;


private:
	UDPSocket* m_mySocket = nullptr;
	UDPSocket* m_theirSocket = nullptr;
	float m_timeOfLastMessageReceive; // Will be used to check for timeouts
};