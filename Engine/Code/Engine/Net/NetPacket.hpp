#pragma once

#include "Engine/Net/NetMessage.hpp"
#include "Engine/Core/BytePacker.hpp"

#define INVALID_PACKET_ACK (0xFFFF)

class NetSession;

struct NetPacketHeader_T {
	uint8_t connectionIndex;

	uint16_t ack = INVALID_PACKET_ACK;
	uint16_t lastRecvdAck = INVALID_PACKET_ACK;
	uint16_t previousRecvdAckBitfield = 0;

	uint8_t messageCount;
};


class NetPacket : public BytePacker {

public:
	NetPacket();
	NetPacket( void* buffer, size_t length );
	~NetPacket();

	void WriteHeader( NetPacketHeader_T header );
	void WriteMessage( NetMessage const& message );

	void ReadHeader( NetPacketHeader_T& out_header );
	NetMessage* ReadMessage( NetSession* session );

private:


};