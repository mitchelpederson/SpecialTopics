#include "Game/PlayerHUD.hpp"
#include "Game/GameCommon.hpp"

//----------------------------------------------------------------------------------------------------------------
PlayerHUD::PlayerHUD() {
	m_gunHitNotificationTimer = new Stopwatch( g_theGame->m_gameClock );
	m_missileHitNotificationTimer = new Stopwatch( g_theGame->m_gameClock );
	m_killNotificationTimer = new Stopwatch( g_theGame->m_gameClock );
}


//----------------------------------------------------------------------------------------------------------------
PlayerHUD::~PlayerHUD() {
	delete m_gunHitNotificationTimer;
	delete m_missileHitNotificationTimer;
	delete m_killNotificationTimer;
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::Update() {
	if ( m_gunHitNotificationTimer->HasElapsed() ) {
		m_gunHitNotification = false;
	}
	if ( m_missileHitNotificationTimer->HasElapsed() ) {
		m_missileHitNotification = false;
	}
	if ( m_killNotificationTimer->HasElapsed() ) {
		m_killNotification = false;
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::NotifyMissileHit() {
	m_missileHitNotification = true;
	m_missileHitNotificationTimer->SetTimer(1.f);
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::NotifyGunHit() {
	m_gunHitNotification = true;
	m_gunHitNotificationTimer->SetTimer(1.f);
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::NotifyKill() {
	m_killNotification = true;
	m_killNotificationTimer->SetTimer(2.f);
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::Render() {
	if ( player != nullptr ) {
		g_theRenderer->SetCameraToUI();
		g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));

		DrawGunReticle();
		DrawPlayerTargetingReticle();
		DrawDirectionToTargetIcon();
		DrawCenterReticle();
		DrawVelocityVectorIcon();
		DrawAltitudeAndSpeed();
		DrawNotifications();
		DrawScoreAndTime();
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::SetPlayerController( EntityController* pc ) {
	player = pc;
	if ( player != nullptr ) {
		playerInfo = g_theGame->GetPlayerInfo( player->connectionID );
	} else {
		playerInfo = nullptr;
	}
}



//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawGunReticle() const {

	if ( player->entity->ValidateLockedEntity() ) {
		Camera* cam = TheGame::GetMultiplayerState()->m_camera;
		Entity* target = TheGame::GetMultiplayerState()->GetEntityByID( player->entity->currentState.lockedEntityID );
		if ( target == nullptr ) {
			return;
		}

		Vector3 displacementToTarget = target->GetPosition() - player->entity->GetPosition();

		Vector3 bulletVelocity = player->entity->GetVelocity() + (player->entity->GetForward() * 400.f);
		float bulletTimeToHitTarget = displacementToTarget.GetLength() / bulletVelocity.GetLength();

		if ( bulletTimeToHitTarget <= 1.f ) {
			Vector3 reticlePos = target->GetPosition() + (target->GetVelocity() * bulletTimeToHitTarget);

			Matrix44 transformation = cam->GetViewProjection();
			Matrix44 toScreen = g_theRenderer->GetClipToScreenSpace( cam );
			Vector4 targetPosTransformed = transformation.Transform( Vector4( reticlePos, 1.f ) );
			Vector3 targetPosAfterW = targetPosTransformed.xyz() * ( 1.f / targetPosTransformed.w );
			Vector4 targetPosScreenSpace = toScreen.Transform( Vector4( targetPosAfterW, 1.f ) );
			Vector3 targetPosScreenSpaceAfterW = targetPosScreenSpace.xyz() * (1.f / targetPosScreenSpace.w);
			Vector2 reticleScreenPos( targetPosScreenSpaceAfterW.x, targetPosScreenSpaceAfterW.y );

			if ( targetPosAfterW.z <= 1.f ) {
				float reticleHalfHeight = 30.f;
				float reticleHalfWidth = reticleHalfHeight;
				AABB2 reticleBounds( reticleScreenPos.x - reticleHalfWidth, reticleScreenPos.y - reticleHalfHeight,
					reticleScreenPos.x + reticleHalfWidth, reticleScreenPos.y + reticleHalfHeight);

				g_theRenderer->DrawTexturedAABB( reticleBounds, *g_theRenderer->CreateOrGetTexture("Data/Images/gun-reticle.png"), Vector2(0.f, 0.f), Vector2(1.f, 1.f), Rgba(255, 255, 255, 255) );
			}
		}

	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawPlayerTargetingReticle() const {
	if ( player->entity->ValidateLockedEntity() ) {
		Camera* cam = TheGame::GetMultiplayerState()->m_camera;

		// If something went wrong and we can't find the target, just don't draw.
		Entity* target = TheGame::GetMultiplayerState()->GetEntityByID( player->entity->currentState.lockedEntityID );
		if ( target == nullptr ) {
			return; 
		}

		// If the target is out of range, don't draw the reticle
		float distanceToTarget = (target->GetPosition() - player->entity->GetPosition()).GetLength();
		if ( distanceToTarget > 10000.f ) {
			return;
		}

		Matrix44 transformation = cam->GetViewProjection();
		Matrix44 toScreen = g_theRenderer->GetClipToScreenSpace( cam );
		Vector4 targetPosTransformed = transformation.Transform( Vector4( target->GetPosition(), 1.f ) );
		Vector3 targetPosAfterW = targetPosTransformed.xyz() * ( 1.f / targetPosTransformed.w );
		Vector4 targetPosScreenSpace = toScreen.Transform( Vector4( targetPosAfterW, 1.f ) );
		Vector3 targetPosScreenSpaceAfterW = targetPosScreenSpace.xyz() * (1.f / targetPosScreenSpace.w);
		Vector2 reticleScreenPos( targetPosScreenSpaceAfterW.x, targetPosScreenSpaceAfterW.y );

		Vector3 disp = target->GetPosition() - player->entity->GetPosition();
		float dotToEnemy = DotProduct( cam->GetForward(), disp.GetNormalized() );
		if ( dotToEnemy > 0.f ) {
			float reticleHalfHeight = 30.f;
			float reticleHalfWidth = reticleHalfHeight;
			AABB2 reticleBounds( reticleScreenPos.x - reticleHalfWidth, reticleScreenPos.y - reticleHalfHeight,
				reticleScreenPos.x + reticleHalfWidth, reticleScreenPos.y + reticleHalfHeight);

			g_theRenderer->DrawTexturedAABB( reticleBounds, *g_theRenderer->CreateOrGetTexture("Data/Images/reticle-jitter.png"), Vector2(0.f, 0.f), Vector2(1.f, 1.f), Rgba(255, 255, 255, 255) );
			
			AABB2 distanceTextBounds( reticleScreenPos.x + reticleHalfWidth, reticleScreenPos.y + (reticleHalfHeight * 0.75f),
										reticleScreenPos.x + reticleHalfWidth + reticleHalfWidth, reticleScreenPos.y - reticleHalfHeight - reticleHalfHeight );
			float distanceToTarget = (target->GetPosition() - player->entity->GetPosition()).GetLength();
			g_theRenderer->DrawTextInBox2D( distanceTextBounds, Vector2(0.f, 0.5f), std::to_string( (int) distanceToTarget), 30.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
			
			int connID = target->controller->connectionID;
			if ( connID != -1 ) {
				AABB2 targetNameBounds( reticleScreenPos.x + reticleHalfWidth, reticleScreenPos.y + (reticleHalfHeight * 0.75f),
										reticleScreenPos.x + reticleHalfWidth + reticleHalfWidth, reticleScreenPos.y + reticleHalfHeight + reticleHalfHeight );
				g_theRenderer->DrawTextInBox2D( targetNameBounds, Vector2(0.f, 0.5f), g_theGame->GetPlayerInfo(connID)->GetName(), 30.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
			}
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawDirectionToTargetIcon() const {
	if ( player->entity->ValidateLockedEntity() ) {
		Entity* target = TheGame::GetMultiplayerState()->GetEntityByID( player->entity->currentState.lockedEntityID );

		Vector3 displacementToTarget = target->GetPosition() - player->entity->GetPosition();
		Vector3 directionToTarget = displacementToTarget.GetNormalized();
		const Transform& myTransform = player->entity->currentState.transform;

		float dotForward = DotProduct( myTransform.GetWorldForward(), directionToTarget );
		if ( dotForward <= 0.95f ) {

			float screenHalfWidth = Window::GetInstance()->GetWidth() * 0.5f;
			float screenHalfHeight = Window::GetInstance()->GetHeight() * 0.5f;

			float dotUp = DotProduct( myTransform.GetWorldUp(), directionToTarget );
			float dotRight = DotProduct( myTransform.GetWorldRight(), directionToTarget );
			Vector2 directionOnHUD( dotRight, dotUp );
			directionOnHUD.NormalizeAndGetLength();
			float orientation = directionOnHUD.GetOrientationDegrees();

			Vector3 topLeft(	 screenHalfWidth + CosDegrees( orientation + 45.f )  * 256.f,  screenHalfHeight + SinDegrees( orientation + 45.f)  * 256.f, 0.f );
			Vector3 topRight(	 screenHalfWidth + CosDegrees( orientation - 45.f )  * 256.f,  screenHalfHeight + SinDegrees( orientation - 45.f)  * 256.f, 0.f  );
			Vector3 bottomLeft(	 screenHalfWidth + CosDegrees( orientation + 135.f ) * 256.f,  screenHalfHeight + SinDegrees( orientation + 135.f) * 256.f, 0.f  );
			Vector3 bottomRight( screenHalfWidth + CosDegrees( orientation - 135.f ) * 256.f,  screenHalfHeight + SinDegrees( orientation - 135.f) * 256.f, 0.f );

			MeshBuilder mb;
			mb.Begin(TRIANGLES, false);

			mb.SetColor(Rgba(255,255,255,255));
			mb.SetUV(Vector2(1.f, 1.f));
			mb.PushVertex(topRight);
			mb.SetUV(Vector2(0.f, 0.f));
			mb.PushVertex(bottomLeft);
			mb.SetUV(Vector2(1.f, 0.f));
			mb.PushVertex(bottomRight);

			mb.SetUV(Vector2(0.f, 0.f));
			mb.PushVertex(bottomLeft);
			mb.SetUV(Vector2(1.f, 1.f));
			mb.PushVertex(topRight);
			mb.SetUV(Vector2(0.f, 1.f));
			mb.PushVertex(topLeft);

			mb.End();

			Mesh quad;
			quad.FromBuilderAsType<Vertex3D_PCU>(&mb);
			g_theRenderer->UseTexture(0, *g_theRenderer->CreateOrGetTexture("Data/Images/target-direction.png"));
			g_theRenderer->DrawMesh(&quad);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawCenterReticle() const {
	Camera* cam = TheGame::GetMultiplayerState()->m_camera;

	Vector3 targetPos = player->entity->GetPosition() + (player->entity->GetForward() * 200.f);

	Matrix44 transformation = cam->GetViewProjection();
	Matrix44 toScreen = g_theRenderer->GetClipToScreenSpace( cam );
	Vector4 targetPosTransformed = transformation.Transform( Vector4( targetPos, 1.f ) );
	Vector3 targetPosAfterW = targetPosTransformed.xyz() * ( 1.f / targetPosTransformed.w );
	Vector4 targetPosScreenSpace = toScreen.Transform( Vector4( targetPosAfterW, 1.f ) );
	Vector3 targetPosScreenSpaceAfterW = targetPosScreenSpace.xyz() * (1.f / targetPosScreenSpace.w);
	Vector2 reticleScreenPos( targetPosScreenSpaceAfterW.x, targetPosScreenSpaceAfterW.y );

	if ( targetPosAfterW.z <= 1.f ) {
		float screenHalfWidth = Window::GetInstance()->GetWidth();
		float screenHalfHeight = Window::GetInstance()->GetHeight();

		float dotUp = DotProduct( Vector3::UP, player->entity->currentState.transform.GetWorldUp() );
		float dotRight = DotProduct( Vector3::UP, player->entity->currentState.transform.GetWorldRight() );
		Vector2 directionOnHUD( dotRight, dotUp );
		directionOnHUD.NormalizeAndGetLength();
		float orientation = directionOnHUD.GetOrientationDegrees();

		Vector3 topLeft(	 reticleScreenPos.x + CosDegrees( orientation + 45.f ) * 55.f,  reticleScreenPos.y + SinDegrees( orientation + 45.f) * 55.f, 0.f );
		Vector3 topRight(	 reticleScreenPos.x + CosDegrees( orientation - 45.f ) * 55.f,  reticleScreenPos.y + SinDegrees( orientation - 45.f) * 55.f, 0.f  );
		Vector3 bottomLeft(	 reticleScreenPos.x + CosDegrees( orientation + 135.f ) * 55.f, reticleScreenPos.y + SinDegrees( orientation + 135.f) * 55.f, 0.f  );
		Vector3 bottomRight( reticleScreenPos.x + CosDegrees( orientation - 135.f ) * 55.f, reticleScreenPos.y + SinDegrees( orientation - 135.f) * 55.f, 0.f );

		MeshBuilder mb;
		mb.Begin(TRIANGLES, false);

		mb.SetColor(Rgba(255,255,255,255));
		mb.SetUV(Vector2(1.f, 1.f));
		mb.PushVertex(topRight);
		mb.SetUV(Vector2(0.f, 0.f));
		mb.PushVertex(bottomLeft);
		mb.SetUV(Vector2(1.f, 0.f));
		mb.PushVertex(bottomRight);

		mb.SetUV(Vector2(0.f, 0.f));
		mb.PushVertex(bottomLeft);
		mb.SetUV(Vector2(1.f, 1.f));
		mb.PushVertex(topRight);
		mb.SetUV(Vector2(0.f, 1.f));
		mb.PushVertex(topLeft);

		mb.End();

		Mesh quad;
		quad.FromBuilderAsType<Vertex3D_PCU>(&mb);
		g_theRenderer->UseTexture(0, *g_theRenderer->CreateOrGetTexture("Data/Images/orientation-icon.png"));
		g_theRenderer->DrawMesh(&quad);
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawVelocityVectorIcon() const {
	Camera* cam = TheGame::GetMultiplayerState()->m_camera;

	Vector3 targetPos = player->entity->GetPosition() + (player->entity->currentState.velocity);

	Matrix44 transformation = cam->GetViewProjection();
	Matrix44 toScreen = g_theRenderer->GetClipToScreenSpace( cam );
	Vector4 targetPosTransformed = transformation.Transform( Vector4( targetPos, 1.f ) );
	Vector3 targetPosAfterW = targetPosTransformed.xyz() * ( 1.f / targetPosTransformed.w );
	Vector4 targetPosScreenSpace = toScreen.Transform( Vector4( targetPosAfterW, 1.f ) );
	Vector3 targetPosScreenSpaceAfterW = targetPosScreenSpace.xyz() * (1.f / targetPosScreenSpace.w);
	Vector2 reticleScreenPos( targetPosScreenSpaceAfterW.x, targetPosScreenSpaceAfterW.y );

	if ( targetPosAfterW.z <= 1.f ) {
		float screenHalfWidth = Window::GetInstance()->GetWidth();
		float screenHalfHeight = Window::GetInstance()->GetHeight();

		float dotUp = DotProduct( Vector3::UP, player->entity->currentState.transform.GetWorldUp() );
		float dotRight = DotProduct( Vector3::UP, player->entity->currentState.transform.GetWorldRight() );
		Vector2 directionOnHUD( dotRight, dotUp );
		directionOnHUD.NormalizeAndGetLength();
		float orientation = directionOnHUD.GetOrientationDegrees();

		Vector3 topLeft(	 reticleScreenPos.x + CosDegrees( orientation + 45.f ) * 70.f,  reticleScreenPos.y + SinDegrees( orientation + 45.f) *  70.f, 0.f );
		Vector3 topRight(	 reticleScreenPos.x + CosDegrees( orientation - 45.f ) * 70.f,  reticleScreenPos.y + SinDegrees( orientation - 45.f) *  70.f, 0.f  );
		Vector3 bottomLeft(	 reticleScreenPos.x + CosDegrees( orientation + 135.f ) * 70.f, reticleScreenPos.y + SinDegrees( orientation + 135.f) * 70.f, 0.f  );
		Vector3 bottomRight( reticleScreenPos.x + CosDegrees( orientation - 135.f ) * 70.f, reticleScreenPos.y + SinDegrees( orientation - 135.f) * 70.f, 0.f );

		MeshBuilder mb;
		mb.Begin(TRIANGLES, false);

		mb.SetColor(Rgba(255,255,255,255));
		mb.SetUV(Vector2(1.f, 1.f));
		mb.PushVertex(topRight);
		mb.SetUV(Vector2(0.f, 0.f));
		mb.PushVertex(bottomLeft);
		mb.SetUV(Vector2(1.f, 0.f));
		mb.PushVertex(bottomRight);

		mb.SetUV(Vector2(0.f, 0.f));
		mb.PushVertex(bottomLeft);
		mb.SetUV(Vector2(1.f, 1.f));
		mb.PushVertex(topRight);
		mb.SetUV(Vector2(0.f, 1.f));
		mb.PushVertex(topLeft);

		mb.End();

		Mesh quad;
		quad.FromBuilderAsType<Vertex3D_PCU>(&mb);
		g_theRenderer->UseTexture(0, *g_theRenderer->CreateOrGetTexture("Data/Images/velocity-icon.png"));
		g_theRenderer->DrawMesh(&quad);
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawAltitudeAndSpeed() const {

	float screenWidth = Window::GetInstance()->GetWidth();
	float screenHeight = Window::GetInstance()->GetHeight();

	AABB2 velLabelBounds( (screenWidth * 0.20f), (screenHeight * 0.5 + 15.f), (screenWidth * 0.25f), (screenHeight * 0.5 + 25.f) );
	AABB2 altLabelBounds( (screenWidth * 0.75f), (screenHeight * 0.5 + 15.f), (screenWidth * 0.80f), (screenHeight * 0.5 + 25.f) );

	AABB2 velBounds( (screenWidth * 0.20f), (screenHeight * 0.5 - 15.f), (screenWidth * 0.25f), (screenHeight * 0.5 + 15.f) );
	AABB2 altBounds( (screenWidth * 0.75f), (screenHeight * 0.5 - 15.f), (screenWidth * 0.80f), (screenHeight * 0.5 + 15.f) );

	g_theRenderer->DrawTextInBox2D( velLabelBounds, Vector2(1.f, 0.5f), "airspeed", 13.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
	g_theRenderer->DrawTextInBox2D( altLabelBounds, Vector2(0.f, 0.5f), "altitude", 13.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );

	g_theRenderer->DrawTextInBox2D( velBounds, Vector2(1.f, 0.5f), std::to_string((int) player->entity->currentState.velocity.GetLength()), 30.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
	g_theRenderer->DrawTextInBox2D( altBounds, Vector2(0.f, 0.5f), std::to_string((int) player->entity->currentState.transform.position.y), 30.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );

	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui"));
	float thrustBarHeight = RangeMapFloat( player->entity->currentState.velocity.GetLength(), 0.f, player->entity->def.GetMaxVelocity(), screenHeight * 0.25f, screenHeight * 0.75f );
	g_theRenderer->DrawAABB(AABB2(screenWidth * 26.f, screenHeight * 0.25f, screenWidth * 30.f, thrustBarHeight), Rgba(182, 255, 118, 220));
	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));

}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawNotifications() const {
	float screenWidth = Window::GetInstance()->GetWidth();
	float screenHeight = Window::GetInstance()->GetHeight();	


	if ( m_gunHitNotification ) {
		std::string gunHit = "GUN HIT";
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, screenHeight * 0.6f, screenWidth, screenHeight * 0.63f), Vector2( 0.5f, 0.5f ), gunHit, 20.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
	}

	if ( m_missileHitNotification ) {
		std::string missileHit = "MISSILE HIT";
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, screenHeight * 0.63f, screenWidth, screenHeight * 0.66f), Vector2( 0.5f, 0.5f ), missileHit, 20.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
	}

	if ( m_killNotification ) {
		std::string kill = "ENEMY DOWN";
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, screenHeight * 0.66f, screenWidth, screenHeight * 0.69f), Vector2( 0.5f, 0.5f ), kill, 20.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
	}
}


//----------------------------------------------------------------------------------------------------------------
void PlayerHUD::DrawScoreAndTime() const {
	float screenWidth = Window::GetInstance()->GetWidth();
	float screenHeight = Window::GetInstance()->GetHeight();	
	
	std::string timeString = std::to_string( g_theGame->GetMultiplayerState()->GetTimeSinceRoundStart() );
	std::string myScoreString = std::to_string( playerInfo->GetScore() );
	std::string myHealthString = std::to_string( (int) player->entity->GetHealth()) + "%";

	g_theRenderer->DrawText2D(Vector2( 10.f, screenHeight - 50.f), timeString, 35.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono") );
	g_theRenderer->DrawText2D(Vector2( 10.f, screenHeight - 90.f), myScoreString, 35.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono") );
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, screenHeight * 0.1f, screenWidth, screenHeight * 0.15f), Vector2( 0.5f, 0.5f ), myHealthString, 50.f, Rgba(182, 255, 118, 220), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN );
}