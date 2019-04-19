#pragma once 
#include "Game/EntityController.hpp"
#include "Game/PlayerInfo.hpp"


class PlayerHUD {
public:
	PlayerHUD();
	~PlayerHUD();

	void Update();
	void Render();

	void SetPlayerController( EntityController* pc );
	void NotifyMissileHit();
	void NotifyGunHit();
	void NotifyKill();

private:
	void DrawGunReticle() const;
	void DrawPlayerTargetingReticle() const;
	void DrawDirectionToTargetIcon() const;
	void DrawCenterReticle() const;
	void DrawVelocityVectorIcon() const;
	void DrawAltitudeAndSpeed() const;
	void DrawScoreAndTime() const;

	void DrawNotifications() const;

	bool m_gunHitNotification = false;
	bool m_missileHitNotification = false;
	bool m_killNotification = false;

	Stopwatch* m_gunHitNotificationTimer = nullptr;
	Stopwatch* m_missileHitNotificationTimer = nullptr;
	Stopwatch* m_killNotificationTimer = nullptr;
	
	EntityController* player = nullptr;
	PlayerInfo* playerInfo = nullptr;

};