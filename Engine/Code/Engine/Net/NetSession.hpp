//----------------------------------------------------------------------------------------------------------------
// NetSession.hpp
// Mitchel Pederson
//
// The idea behind this implementation is that the host will manage all client information and
//	disperse this information regularly to all other clients. Session control messages will only
//	be handled between client and host, never client to client. Game messages may be sent client
//	to client however,
//
//----------------------------------------------------------------------------------------------------------------


#pragma once
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetConnection.hpp"

#include <map>

#define GAME_PORT 10084
#define CLIENT_SYNC_MAX_TIME 10

typedef bool (*net_message_cb)( NetMessage const& message, NetConnection const& sender );

enum eNetSessionState {
	NET_STATE_INIT,
	NET_STATE_JOINING,
	NET_STATE_JOINED,
	NET_STATE_HOSTING,
	NET_STATE_DISCONNECTING
};


class NetSession {

public:
	NetSession();
	~NetSession();


	//----------------------------------------------------------------------------------------------------------------
	// Initialization
	bool AddBinding( unsigned short session );
	void RegisterMessage( std::string const& name, net_message_cb* callback );


	//----------------------------------------------------------------------------------------------------------------
	// Updates
	void ValidateConnections(); // Will try to ping clients that haven't been heard from in a while and disconnect them if they time out
	void ProcessOutgoing();		// Tries to send all messages in the queue - At the end of Game's update
	void ProcessIncoming();		// Does receives, unpacks the messages and fires the callbacks if needed - beginning of game update


	//----------------------------------------------------------------------------------------------------------------
	// Session control
	
	// If we are a client, this will ask the host to send us an updated list of all the
	//   clients in the session. 
	void RequestConnectionInformationFromHost(); 

	// If we are a host, this will send info about all clients to everyone so that they all know
	//   about each other.
	void BroadcastNewConnectionInfo();

	// If we are a client and we received session info from the host about what other clients there
	//   are, then we will update m_connections to match. This may result in attempting to form
	//   new connections and deleting old ones that timed out or were closed
	void UpdateClientInformation( NetMessage const& clientInfoMessage );

	// If we are a client, this will tell the host that we disconnected. Doesn't worry about other clients, as
	//   the host will update them on the next client sync.
	// If we are a host, this will message to all clients that the session is ending.
	void Disconnect(); 


	//----------------------------------------------------------------------------------------------------------------
	// Registered control messages

	// Simple ping to see if someone timed out
	static bool AskIfConnected( NetMessage const& message, NetConnection const& destination ); 

	// clientInfo holds all other client information for the session
	static bool SendClientInfoUpdateToClient( NetMessage const& clientInfo, NetConnection const& destination );
	
	// Tells someone else that I want to disconnect
	static bool NotifyDisconnection( NetMessage const& message, NetConnection const& destination );

	//----------------------------------------------------------------------------------------------------------------
	// Sending messages
	void SendMessageTo( NetMessage const& message, NetConnection const& destination );
	void BroadcastMessageToSession( NetMessage const& message );


	//----------------------------------------------------------------------------------------------------------------
	// Processing received messages
	net_message_cb* GetCallbackForMessage( NetMessage const& message ); // can return nullptr if registered message isn't found


	//----------------------------------------------------------------------------------------------------------------
	// Session information queries
	NetConnection*	GetConnectionByIndex( unsigned int index );
	unsigned int	GetNumberOfUsers() const;


private:
	// If we are hosting, this is our connection info.
	// If we are joined, this is the connection to the host.
	NetConnection* m_hostConnection = nullptr; 

	std::vector< NetConnection* > m_connections;				 // All of the clients I know about in either state
	std::map<std::string, net_message_cb*> m_registeredMessages; // The messages we know how to handle with cbs
	std::queue< NetMessage* > m_outgoingMessages;				 // Stores outgoing messages to be handled in ProcessOutgoing()
	eNetSessionState m_state;

	float m_timeOfLastClientSync;	// periodically send or request a client info sync
};