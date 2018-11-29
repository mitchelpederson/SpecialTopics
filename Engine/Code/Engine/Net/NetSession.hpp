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
#define DEFAULT_PORT_RANGE 32
#define CLIENT_SYNC_MAX_TIME 10
#define MAX_CLIENTS 32
#define MTU (1500-48)
#define UNRELIABLE_RESEND_TIME 0.1f
#define MAX_RELIABLES_PER_PACKET 32
#define JOIN_TIMEOUT 10.f
#define MAX_NET_TIME_DILATION 0.1f


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
	SESSION_DISCONNECTED = 0,
	SESSION_BOUND,
	SESSION_CONNECTING,
	SESSION_JOINING,
	SESSION_READY	
};


enum eNetSessionError {
	SESSION_OK,

	SESSION_ERROR_USER,
	SESSION_ERROR_JOIN_DENIED,
	SESSION_ERROR_JOIN_DENIED_NOT_HOST,
	SESSION_ERROR_JOIN_DENIED_CLOSED,
	SESSION_ERROR_JOIN_DENIED_FULL
};


enum eNetCoreMessage {
	NETMSG_PING = 0,
	NETMSG_PONG,
	NETMSG_HEARTBEAT,

	NETMSG_JOIN_REQUEST,
	NETMSG_JOIN_DENY,
	NETMSG_JOIN_ACCEPT,
	NETMSG_NEW_CONNECTION,
	NETMSG_JOIN_FINISHED,
	NETMSG_UPDATE_CONN_STATE,
	NETMSG_HANGUP,
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

	void Update();				// Called at the beginning of ProcessOutgoing()

	//----------------------------------------------------------------------------------------------------------------
	// Session control
	NetConnection* AddConnection( NetConnectionInfo_T const& info );
	NetConnection* GetConnection( unsigned int index );
	void DestroyConnection( NetConnection* conn );
	void BindConnection( uint8_t index, NetConnection* conn );

	void SetConnectionTickRate( int index, float rate );
	void SetHeartbeatRate( float hz );
	void SetState( eNetSessionState state );

	void Host( std::string const& myID, uint16_t port, uint16_t portRange = DEFAULT_PORT_RANGE );
	void Join( std::string const& myID, NetConnectionInfo_T const& hostInfo );
	void Disconnect();

	void CloseSocket();

	static void SetTickRateCommand( std::string const& command );


	//----------------------------------------------------------------------------------------------------------------
	// Session error processing
	void SetError( eNetSessionError error, std::string str = "" );
	void ClearError();
	eNetSessionError GetLastNetError();


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
	unsigned int	GetNumberOfConnections() const;
	uint8_t			GetMyConnectionIndex() const;
	NetAddress_T	GetMyAddress() const;
	UDPSocket*		GetSocket() const;
	float			GetTimeSinceLastMessageOnConnection( unsigned int connIndex );
	NetConnection*	GetMyConnection() const;
	NetConnection*	GetHostConnection() const;

	bool			IsConnected() const;
	bool			IsReady() const;
	bool			IsDisconnected() const;
	bool			IsBound() const;

	bool			AmIHost() const;

	bool			IsAddressAlreadyConnected( NetAddress_T const& addr );
	uint8_t			GetNextFreeSessionID() const;
	eNetSessionState GetState() const;

	
	//----------------------------------------------------------------------------------------------------------------
	// Control message callbacks
	static bool OnHeartbeat( NetMessage& message, NetConnection& sender );
	static bool OnCoreDummy( NetMessage& message, NetConnection& sender );


	//----------------------------------------------------------------------------------------------------------------
	// Sim control
	void SetSimLatency( float min, float max );
	void SetSimLossRate( float rate );
	static void SetSimLossRateCommand( std::string const& command );
	static void SetSimLatencyCommand( std::string const& command );
	float GetSimLossRate() const;
	FloatRange GetSimLatency() const;


	//----------------------------------------------------------------------------------------------------------------
	// Net Clock 
	void SetHostTime( double hostTime );
	void InitializeTimesFromHost( double hostTime );
	double GetNetTime();
	double GetHostTime();


private:
	bool AddBinding( unsigned short session );


public:
	static NetSession* instance;
	static Clock* m_sessionClock;


private:
	UDPSocket*							m_socket = nullptr;
	NetConnection*						m_myConnection = nullptr;
	NetConnection*						m_hostConnection = nullptr;
	std::list< NetConnection* >			m_allConnections;			// All of the clients I know about in either state
	std::vector< NetConnection* >		m_boundConnections;
	static std::vector< NetCommand >	m_registeredMessages;	// The messages we know how to handle with cbs
	
	eNetSessionState					m_state = SESSION_DISCONNECTED;
	eNetSessionError					m_error = SESSION_OK;
	std::string							m_errorString = "";

	std::vector< PacketInLatencySim_T* > m_incomingPackets;

	static Stopwatch m_joinTimer;

	static FloatRange m_simLatency;
	static float m_simLossRate;		// [0, 1], 0 being no loss and 1 being complete loss
	static float m_tickRate;
	static Stopwatch m_sessionTick;


	static double m_lastReceivedHostTime;
	static double m_desiredClientTime;
	static double m_currentClientTime;
	static float m_deltaTimeDilation;
};