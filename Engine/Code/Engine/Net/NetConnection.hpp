//----------------------------------------------------------------------------------------------------------------
// NetConnection.hpp
// Mitchel Pederson
//
// We should probably queue the sends here rather than on NetSession
// 
//----------------------------------------------------------------------------------------------------------------

#pragma once 
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetMessage.hpp"
#include "Engine/Net/NetPacket.hpp"
#include "Engine/Net/TrackedPacket.hpp"

#include "Engine/Core/Stopwatch.hpp"

#include <queue>
#include <list>


#define MAX_TRACKED_HISTORY_SIZE 128
#define DEFAULT_HEARTBEAT 2.f
#define RELIABLE_WINDOW 32


class NetSession;

class NetConnection {

public:
	NetConnection( NetSession* session, uint8_t connectionIndex, NetAddress_T const& address );
	~NetConnection();

	void	Send( NetMessage& message );
	int		SendPacket( UDPSocket* socketToSendFrom );
	int		SendPacketImmediate( UDPSocket* socketToSendFrom, NetMessage& message, bool isAckConfirm = false );
	void	Receive( NetMessage* message );
	void	ProcessIncoming( NetPacket& packet );

	void	UpdateHeartbeat();
	void	SetSendRate( float rate );

	// Acks and Reliables
	TrackedPacket*	AddTrackedPacket( NetPacket* packet, uint8_t ack );
	uint16_t		GetNextAckToSend();
	void			IncrementNextAckToSend();
	void			ConfirmPacketReceived( uint16_t ack );


	float GetTimeSinceLastReceivedMessage() const;
	std::string GetAddressAsString() const;
	NetAddress_T GetAddress() const;
	bool HasTickElapsed();
	uint16_t GetNextSentAck();
	uint16_t GetLastRecvdAck();
	uint16_t GetPreviousRecvdAckBitfield();
	uint8_t GetConnectionIndex();
	float GetRTT();
	float GetLoss();
	float GetTimeAtLastReceive();
	float GetTimeAtLastSend();
	uint16_t GetNumUnconfirmedReliables();

public:
	NetSession* m_session = nullptr;


private:
	bool IsReliableUnconfirmed( uint16_t reliable );
	uint16_t GetOldestUnconfirmedReliable();
	bool CanSendNewReliable();
	void AddToReceivedReliablesList( uint16_t reliable );

private:
	uint8_t m_connectionIndex;
	NetAddress_T m_remoteAddress;
	float m_timeAtLastReceive = 0.f;
	float m_timeAtLastSend = 0.f;
	float m_rtt = 0.f;
	float m_loss = 0.f;
	Stopwatch m_sendTick;
	Stopwatch m_heartbeat;

	std::queue<NetMessage*> m_unsentReliables;
	std::vector<NetMessage*> m_unconfirmedReliables;
	std::queue<NetMessage*> m_outgoingUnreliables;
	std::queue<NetMessage*> m_incomingMessages;

	// ack members
	uint16_t m_nextSentAck = 0U;
	uint16_t m_highestRecvdAck = INVALID_PACKET_ACK;
	uint16_t m_previousRecvdAckBitfield = 0U;
	TrackedPacket* m_trackedPackets[MAX_TRACKED_HISTORY_SIZE];

	// reliable members
	uint16_t m_lastSentReliable = 0;
	std::list<uint16_t> m_receivedReliables;
	uint16_t m_oldestUnconfirmedReliable = 0;
	uint16_t m_highestReceivedReliable = 0;

};