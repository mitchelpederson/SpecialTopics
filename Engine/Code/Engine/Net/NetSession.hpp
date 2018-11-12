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
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/Stopwatch.hpp"

#include <map>

#define GAME_PORT 10084
#define CLIENT_SYNC_MAX_TIME 10
#define MAX_CLIENTS 32
#define MTU (1500-48)
#define UNRELIABLE_RESEND_TIME 0.1f
#define MAX_RELIABLES_PER_PACKET 32

typedef bool (*net_message_cb)( NetMessage& message, NetConnection& sender );

enum eNetMessageFlag {
	NETMSG_OPTION_CONNECTIONLESS = 0x01,
	NETMSG_OPTION_RELIABLE = 0x02,
	NETMSG_OPTION_IN_ORDER = 0x06	// In order traffic is always reliable
};


struct NetCommand {
	uint8_t id = 0xFF;
	std::string name = "null";
	net_message_cb callback = nullptr;
	uint16_t flags = 0;
	uint8_t channel = 0;

	bool RequiresConnection()	{ return !(flags & NETMSG_OPTION_CONNECTIONLESS); }
	bool IsReliable()			{ return (flags & NETMSG_OPTION_RELIABLE) == NETMSG_OPTION_RELIABLE; }
	bool IsInOrder()			{ return (flags & NETMSG_OPTION_IN_ORDER) == NETMSG_OPTION_IN_ORDER; }
};


enum eNetSessionState {
	NET_STATE_INIT,
	NET_STATE_JOINING,
	NET_STATE_JOINED,
	NET_STATE_HOSTING,
	NET_STATE_DISCONNECTING
};


enum eNetCoreMessage {
	NETMSG_PING = 0,
	NETMSG_PONG,
	NETMSG_HEARTBEAT,
	NETMSG_CORE_COUNT
};


struct PacketInLatencySim_T {
	NetPacket* packet;
	NetAddress_T addr;
	float timestamp;

	~PacketInLatencySim_T() {
		delete packet;
		packet = nullptr;
	}
};


class NetSession {

public:
	NetSession();
	~NetSession();


	//----------------------------------------------------------------------------------------------------------------
	// Initialization
	bool AddBinding( unsigned short session );
	void RegisterMessage( uint8_t index, std::string const& name, net_message_cb callback, uint16_t flags = 0, uint8_t channel = 0 );
	void RegisterCoreMessages();


	//----------------------------------------------------------------------------------------------------------------
	// Updates
	void ValidateConnections(); // Will try to ping clients that haven't been heard from in a while and disconnect them if they time out
	void ProcessOutgoing();		// Tries to send all messages in the queue - At the end of Game's update
	void ProcessIncoming();		// Does receives, unpacks the messages and fires the callbacks if needed - beginning of game update
	void ProcessPacket( PacketInLatencySim_T* packet );
	void AddPacketToLatencyQueue( NetPacket* packet, NetAddress_T addr );
	void ProcessLatencyQueue();

	//----------------------------------------------------------------------------------------------------------------
	// Session control
	NetConnection* AddConnection( unsigned int index, NetAddress_T address );
	NetConnection* GetConnection( unsigned int index );
	void SetNetIndex( uint8_t index );
	static void SetTickRateCommand( std::string const& command );
	void SetConnectionTickRate( int index, float rate );
	void SetHeartbeatRate( float hz );


	//----------------------------------------------------------------------------------------------------------------
	// Processing received messages
	static net_message_cb GetCallbackForMessage( uint8_t index ); // can return nullptr if registered message isn't found
	static uint8_t GetMessageIndexForName( std::string const& name );
	static NetCommand GetCommand( uint8_t index );


	//----------------------------------------------------------------------------------------------------------------
	// Packet/Message Validation
	bool IsValidConnectionIndex( uint8_t connectionIndex );


	//----------------------------------------------------------------------------------------------------------------
	// Session information queries
	unsigned int	GetNumberOfUsers() const;
	uint8_t			GetMyConnectionIndex() const;
	NetAddress_T	GetMyAddress() const;
	UDPSocket*		GetSocket() const;
	float			GetTimeSinceLastMessageOnConnection( unsigned int connIndex );
	NetConnection*	GetMyConnection() const;

	
	//----------------------------------------------------------------------------------------------------------------
	// Control message callbacks
	static bool OnHeartbeat( NetMessage& message, NetConnection& sender );


	//----------------------------------------------------------------------------------------------------------------
	// Sim control
	void SetSimLatency( float min, float max );
	void SetSimLossRate( float rate );
	static void SetSimLossRateCommand( std::string const& command );
	static void SetSimLatencyCommand( std::string const& command );
	float GetSimLossRate() const;
	FloatRange GetSimLatency() const;


private:
	uint8_t								m_myConnection;
	UDPSocket*							m_socket = nullptr;
	std::vector< NetConnection* >		m_connections;			// All of the clients I know about in either state
	static std::vector< NetCommand >	m_registeredMessages;	// The messages we know how to handle with cbs
	eNetSessionState					m_state;

	std::vector< PacketInLatencySim_T* > m_incomingPackets;

	static FloatRange m_simLatency;
	static float m_simLossRate;		// [0, 1], 0 being no loss and 1 being complete loss
	static float m_tickRate;
	static Stopwatch m_sessionTick;
};