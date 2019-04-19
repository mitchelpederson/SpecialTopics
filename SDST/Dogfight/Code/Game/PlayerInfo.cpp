#include "Game/PlayerInfo.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
PlayerInfo::PlayerInfo( uint8_t connID ) 
	: m_connectionID( connID )
{

}


//----------------------------------------------------------------------------------------------------------------
void PlayerInfo::SetName( const std::string& name ) {
	m_name = name;
}


//----------------------------------------------------------------------------------------------------------------
void PlayerInfo::GivePoints( int points ) {
	m_roundScore += points;

	Logger::PrintTaggedf("Net", "Adding %d points to player %d", points, m_connectionID );
}


//----------------------------------------------------------------------------------------------------------------
void PlayerInfo::RecordGunHit() {
	m_roundGunHits++;
	Logger::PrintTaggedf("Net", "Adding gun hit to player %d", m_connectionID );
	GivePoints(70);
}


//----------------------------------------------------------------------------------------------------------------
void PlayerInfo::RecordMissileHit() {
	m_roundMissileHits++;
	Logger::PrintTaggedf("Net", "Adding missile hit to player %d", m_connectionID );

	GivePoints(700);
}


//----------------------------------------------------------------------------------------------------------------
void PlayerInfo::RecordKill() {
	m_roundKills++;
	Logger::PrintTaggedf("Net", "Adding kill to player %d", m_connectionID );
	GivePoints(700);
}


//----------------------------------------------------------------------------------------------------------------
std::string PlayerInfo::GetName() const {
	return m_name;
}


//----------------------------------------------------------------------------------------------------------------
uint8_t PlayerInfo::GetConnectionID() const {
	return m_connectionID;
}


//----------------------------------------------------------------------------------------------------------------
int PlayerInfo::GetScore() const {
	return m_roundScore;
}


//----------------------------------------------------------------------------------------------------------------
int PlayerInfo::GetKills() const {
	return m_roundKills;
}


//----------------------------------------------------------------------------------------------------------------
int PlayerInfo::GetMissileHits() const {
	return m_roundMissileHits;
}


//----------------------------------------------------------------------------------------------------------------
int PlayerInfo::GetGunHits() const {
	return m_roundGunHits;
}


//----------------------------------------------------------------------------------------------------------------
void PlayerInfo::UpdateFromSnapshot( PlayerInfoSnapshot_T* snapshot ) {

	if ( m_roundKills < snapshot->kills ) {
		g_theGame->GetMultiplayerState()->hud->NotifyKill();
	}

	if ( m_roundMissileHits < snapshot->missileHits ) {
		g_theGame->GetMultiplayerState()->hud->NotifyMissileHit();
	}

	if ( m_roundGunHits < snapshot->gunHits ) {
		g_theGame->GetMultiplayerState()->hud->NotifyGunHit();
	}

	m_roundScore = snapshot->score;
	m_roundKills = snapshot->kills;
	m_roundMissileHits = snapshot->missileHits;
	m_roundGunHits = snapshot->gunHits;
}


//----------------------------------------------------------------------------------------------------------------
void PlayerInfo::Reset() {
	m_roundGunHits = 0;
	m_roundKills = 0;
	m_roundMissileHits = 0;
	m_roundScore = 0;
}