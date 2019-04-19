#pragma once
#include "Engine/Net/NetMessage.hpp"

#include <inttypes.h>
#include <string>


struct PlayerInfoSnapshot_T {
	int score;
	int kills;
	int gunHits;
	int missileHits;
	float timestamp;
};


class PlayerInfo {
	
public:
	PlayerInfo( uint8_t connID );

	void		SetName( const std::string& name );
	void		GivePoints( int points );
	void		RecordGunHit();
	void		RecordMissileHit();
	void		RecordKill();
	void		UpdateFromSnapshot( PlayerInfoSnapshot_T* snapshot );
	void		Reset();

	std::string GetName() const;
	uint8_t		GetConnectionID() const;
	int			GetScore() const;
	int			GetKills() const;
	int			GetMissileHits() const;
	int			GetGunHits() const;


private:


public:


private:
	uint8_t m_connectionID;
	std::string m_name = "DEFAULT";

	int m_roundScore = 0;
	int m_roundKills = 0;
	int m_roundGunHits = 0;
	int m_roundMissileHits = 0;

};



void	SendPlayerInfoCreate( NetMessage* msg, void* obj );
void*	RecvPlayerInfoCreate( NetMessage* msg );
void	SendPlayerInfoDestroy( NetMessage* msg, void* obj );
void	RecvPlayerInfoDestroy( NetMessage* msg, void* obj );

void	GetPlayerInfoSnapshot( void*& snapshot, void* obj );
void	SendPlayerInfoSnapshot( NetMessage* msg, void* snapshot );
void	RecvPlayerInfoSnapshot( NetMessage* msg, void* snapshot );
void	ApplyPlayerInfoSnapshot( void* snapshot, void* obj, float snapshotAge );