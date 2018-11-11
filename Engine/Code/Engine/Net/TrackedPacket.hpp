#pragma once

#include "Engine/Net/NetPacket.hpp"

#define MAX_RELIABLES_PER_PACKET 32

class TrackedPacket {

public:
	void SetPacket( NetPacket* packet );
	void SetTimeSent( float seconds );
	void AddSentReliable( uint16_t id );
	uint8_t GetIndex();
	void Invalidate();
	bool IsValid();
	float GetTimeSent();
	uint8_t GetNumReliablesInPacket();
	uint16_t* GetSentReliablesArray();


private:
	NetPacket* m_packet = nullptr;
	uint8_t m_index;
	bool m_isValid = true;
	float m_timeSent = 0.f;
	uint16_t m_sentReliables[ MAX_RELIABLES_PER_PACKET ];
	uint8_t m_reliablesInPacket = 0;
};