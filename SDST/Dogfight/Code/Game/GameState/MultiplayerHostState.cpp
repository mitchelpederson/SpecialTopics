#include "Game/GameState/MultiplayerHostState.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/Renderer/DebugRender.hpp"


//----------------------------------------------------------------------------------------------------------------
MultiplayerHostState::MultiplayerHostState() {
	netSession = g_theGame->netSession;
}



//----------------------------------------------------------------------------------------------------------------
void MultiplayerHostState::OnEnter() {
	BeginFadeIn(0.5f);
	Initialize();
	
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerHostState::OnBeginExit() {
	BeginFadeOut(0.5f);
}


void MultiplayerHostState::Update() {
	PROFILER_SCOPED_PUSH();

	if (currentSubstate == HOST_STATE_PLAYING) {

		if ( g_theInputSystem->WasKeyJustPressed('M') ) {
			Entity* aiPlane = CreateEntity( 1 );
			aiPlane->Spawn();
			aiPlane->currentState.transform.position = Vector3( GetRandomFloatInRange(-2000.f, 2000.f), 0.f, 0.f );
		}

		if ( (g_theInputSystem->WasKeyJustPressed('P') || g_theInputSystem->GetController(0).WasButtonJustPressed(InputSystem::XBOX_START))
			&& localPlayer == nullptr ) {
			SpawnPlayer();
		}

		if ( netSession->IsReady() && localPlayer == nullptr ) {
			Entity* localPlayerEntity = FindPlayerByConnection( netSession->GetMyConnectionIndex() );
			if ( localPlayerEntity != nullptr ) {
				localPlayer = localPlayerEntity->controller;
				localPlayerEntity->followCamera = m_camera;
			}
		}
		
		UpdateEntitiesAndControllers();
		CheckEntityCollisions();
		ClearDeadEntities();
	
		m_camera->Update();
		hud->Update();

		m_terrain->Update();
		CheckWinConditions();
	}
	if ( currentSubstate == HOST_STATE_LOSS || currentSubstate == HOST_STATE_VICTORY ) {
		if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_ENTER ) 
			|| g_theInputSystem->GetController(0).WasButtonJustPressed(InputSystem::XBOX_START) ) {
		
			g_theGame->BeginTransitionToState( STATE_MENU_HOSTING );

			NetMessage msg( NETMSG_RETURN_TO_MENU );
			g_theGame->netSession->SendToAllOtherConnections(msg);
		}
	}

	GameState::Update();
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerHostState::Render() {
	PROFILER_SCOPED_PUSH();

	g_theRenderer->DisableAllLights();
	// setup
	float halfWidth = Window::GetInstance()->GetWidth() * 0.5f;
	float halfHeight = Window::GetInstance()->GetHeight() * 0.5f;

	g_theRenderer->SetCamera(m_camera);
	m_camera->SetProjection(Matrix44::MakeProjection(60.f, halfWidth / halfHeight, 1.f, 100000.f));
	g_theRenderer->SetAmbientLight(ambientIntensity, ambientColor);
	g_theRenderer->SetSpecular(specularPower, specularAmount);
	m_forwardRenderPath->RenderSceneForCamera( m_camera, m_scene );

	g_theRenderer->SetCameraToUI();

	if ( currentSubstate == HOST_STATE_PLAYING ) {
		if ( localPlayer != nullptr ) {
			hud->Render();
			if ( m_debugDraw ) {
				DrawScaleGridAroundPlayer();
			}
		}

		else {
			g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui"));
			g_theRenderer->DrawAABB(AABB2(0.f, 0.f, halfWidth * 2.f, halfHeight * 2.f), Rgba(0, 0, 0, 100));

			g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));
			g_theRenderer->DrawTextInBox2D( AABB2( halfWidth * 0.5f, halfHeight * 0.5f, halfWidth * 1.5f, halfHeight * 1.5f ), Vector2( 0.5f, 0.5f ), "Press Start or P to spawn", 100.f, Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_SHRINK_TO_FIT );

		}
	}

	DrawEndScreen();

	DebugRenderSet3DCamera( m_camera );
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerHostState::DrawEndScreen() {
	if ( currentSubstate != HOST_STATE_PLAYING ) {

		float halfWidth = Window::GetInstance()->GetWidth() * 0.5f;
		float halfHeight = Window::GetInstance()->GetHeight() * 0.5f;

		std::string text = "";
		std::string yourScoreText = "Your score: " + std::to_string( g_theGame->GetMyPlayerInfo()->GetScore());
		std::string topScoreText = "Top score: " + std::to_string( m_winnerInfo->GetScore() );
		
		if ( currentSubstate == HOST_STATE_VICTORY ) {
			text = "You are the top ace!";
		} else {
			text = "You lost. " + m_winnerInfo->GetName() + " was the top ace.";
		}

		g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui"));
		g_theRenderer->DrawAABB(AABB2(0.f, 0.f, halfWidth * 2.f, halfHeight * 2.f), Rgba(0, 0, 0, 100));

		g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));
		g_theRenderer->DrawTextInBox2D( AABB2( halfWidth * 0.5f, halfHeight, halfWidth * 1.5f, halfHeight * 1.5f ), Vector2( 0.5f, 0.f ), text, 100.f, Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_SHRINK_TO_FIT );
		g_theRenderer->DrawTextInBox2D( AABB2( halfWidth * 0.5f, halfHeight * 0.75f, halfWidth * 1.5f, halfHeight ), Vector2( 0.5f, 0.f ), topScoreText, 50.f, Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_SHRINK_TO_FIT );
		g_theRenderer->DrawTextInBox2D( AABB2( halfWidth * 0.5f, halfHeight * 0.5f,  halfWidth * 1.5f, halfHeight * 0.75), Vector2( 0.5f, 0.f ), yourScoreText, 50.f, Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_SHRINK_TO_FIT );
	}
}



//----------------------------------------------------------------------------------------------------------------
void MultiplayerHostState::SpawnPlayer() {
	Entity* p = CreateEntity( 0, new PlayerController(), netSession->GetMyConnectionIndex() );
	p->Spawn();
	p->followCamera = m_camera;
	localPlayer = p->controller;
	hud->SetPlayerController( p->controller );
}


//----------------------------------------------------------------------------------------------------------------
Entity* MultiplayerHostState::CreateEntity( int entityDefID, EntityController* controller /* = nullptr */, int connectionIndex /* = 0 */ ) {
	Entity* ent = new Entity( *EntityDefinition::s_definitions[ entityDefID ] );
	ent->currentState.id = entityIdCounter;
	entities[ ent->currentState.id ] = ent;

	if ( controller != nullptr ) {
		controller->AssignToEntity( ent );
		controllers[ ent->currentState.id ] = controller;
		if ( connectionIndex >= 0 ) {
			controller->connectionID = (uint8_t) connectionIndex;
		}
	} else {
		EntityController* entController = new EntityController(); // default controller
		entController->AssignToEntity( ent );

		controllers[ ent->currentState.id ] = entController;
	}

	netSession->netObjectSystem->SyncObject( 1, (void*) ent );
	entityIdCounter++;
	return ent;
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerHostState::CheckWinConditions() {
	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		PlayerInfo* info = g_theGame->GetPlayerInfo( i );
		if ( info != nullptr ) {

			if ( info->GetScore() > MATCH_SCORE_LIMIT ) {
				m_winnerInfo = info;
				if ( g_theGame->GetMyPlayerInfo()->GetConnectionID() == m_winnerInfo->GetConnectionID() ) {
					currentSubstate = HOST_STATE_VICTORY;
				} else {
					currentSubstate = HOST_STATE_LOSS;
				}
			}
		}
	}

	if ( GetTimeSinceRoundStart() > MATCH_TIME_LIMIT ) {

		// Find the player with the highest score
		int maxScore = 0;
		for ( int i = 0; i < MAX_CLIENTS; i++ ) {
			PlayerInfo* info = g_theGame->GetPlayerInfo( i );
			if ( info != nullptr ) {
				if ( info->GetScore() > maxScore ) {
					m_winnerInfo = info;
					maxScore = info->GetScore();
				}
			}
		}

		if ( g_theGame->GetMyPlayerInfo()->GetConnectionID() == m_winnerInfo->GetConnectionID() ) {
			currentSubstate = HOST_STATE_VICTORY;
		} else {
			currentSubstate = HOST_STATE_LOSS;
		}
	}

	// If we have a winner send a round end message to the clients.
	if ( m_winnerInfo != nullptr ) {
		NetMessage msg( NETMSG_ROUND_END );
		msg.WriteValue<uint8_t>( m_winnerInfo->GetConnectionID() );
		g_theGame->netSession->SendToAllOtherConnections(msg);
	}
}