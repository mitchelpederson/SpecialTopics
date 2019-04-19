#include "Game/GameState/MultiplayerClientState.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"



//----------------------------------------------------------------------------------------------------------------
MultiplayerClientState::MultiplayerClientState() {

}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerClientState::OnEnter() {
	BeginFadeIn(0.5f);
	Initialize();
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerClientState::OnBeginExit() {
	BeginFadeOut(0.5f);
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerClientState::Update() {
	PROFILER_SCOPED_PUSH();

	if ( g_theInputSystem->WasKeyJustPressed('V') ) {
		m_debugDraw =  !m_debugDraw;
	}

	if (currentSubstate == CLIENT_STATE_PLAYING) {
		if ( (g_theInputSystem->WasKeyJustPressed('P') || g_theInputSystem->GetController(0).WasButtonJustPressed(InputSystem::XBOX_START))
			&& localPlayer == nullptr ) {
			SpawnPlayer();
		}

		if ( netSession->IsReady() ) {

			if ( localPlayer == nullptr ) {
				Entity* localPlayerEntity = FindPlayerByConnection( netSession->GetMyConnectionIndex() );
				if ( localPlayerEntity != nullptr ) {
					localPlayer = localPlayerEntity->controller;
					localPlayerEntity->followCamera = m_camera;
					hud->SetPlayerController( localPlayer );
				}
			}

		}

		UpdateEntitiesAndControllers();
		ClearDeadEntities();

		if ( localPlayer != nullptr ) {
			NetMessage controllerUpdate(NETMSG_UPDATE_REMOTE_CONTROLLER);
			controllerUpdate.WriteValue<int>( localPlayer->connectionID );
			controllerUpdate.WriteValue<float>( localPlayer->throttle );
			controllerUpdate.WriteValue<float>( localPlayer->rollAxis );
			controllerUpdate.WriteValue<float>( localPlayer->pitchAxis );
			controllerUpdate.WriteValue<float>( localPlayer->yawAxis );
			controllerUpdate.WriteValue<bool>( localPlayer->isFireMissilePressed );
			controllerUpdate.WriteValue<bool>( localPlayer->isFireGunPressed );

			netSession->GetHostConnection()->Send( controllerUpdate );
		}

		m_camera->Update();
		hud->Update();
	}

	GameState::Update();
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerClientState::Render() {
	PROFILER_SCOPED_PUSH();

	g_theRenderer->DisableAllLights();
	// setup
	float halfWidth = Window::GetInstance()->GetWidth() / 2.f;
	float halfHeight = Window::GetInstance()->GetHeight() / 2.f;

	g_theRenderer->SetCamera(m_camera);
	m_camera->SetProjection(Matrix44::MakeProjection(40.f, halfWidth / halfHeight, 0.001f, 10000.f));
	g_theRenderer->SetAmbientLight(ambientIntensity, ambientColor);
	g_theRenderer->SetSpecular(specularPower, specularAmount);
	m_forwardRenderPath->RenderSceneForCamera( m_camera, m_scene );

	g_theRenderer->SetCameraToUI();
	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui"));

	if ( currentSubstate == CLIENT_STATE_PLAYING ) {
		if ( localPlayer != nullptr ) {
			hud->Render();
			if ( m_debugDraw ) {
				//DrawScaleGridAroundPlayer();
			}
		}

		else {
			g_theRenderer->DrawAABB(AABB2(0.f, 0.f, halfWidth * 2.f, halfHeight * 2.f), Rgba(0, 0, 0, 100));

			g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));
			g_theRenderer->DrawTextInBox2D( AABB2( halfWidth * 0.5f, halfHeight * 0.5f, halfWidth * 1.5f, halfHeight * 1.5f ), Vector2( 0.5f, 0.5f ), "Press Start or P to spawn", 100.f, Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_SHRINK_TO_FIT );

		}
	}

	DrawEndScreen();
	DebugRenderSet3DCamera( m_camera );
}

//----------------------------------------------------------------------------------------------------------------
void MultiplayerClientState::DrawEndScreen() {
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
		g_theRenderer->DrawTextInBox2D( AABB2( halfWidth * 0.5f, halfHeight * 0.5f, halfWidth * 1.5f, halfHeight * 0.75), Vector2( 0.5f, 0.f ), yourScoreText, 50.f, Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_SHRINK_TO_FIT );

	}
}



//----------------------------------------------------------------------------------------------------------------
Entity* MultiplayerClientState::CreateEntityFromNet( int id, int entityDefID, EntityController* controller /* = nullptr */, int connectionIndex /* = 0 */ ) {
	Entity* ent = new Entity( *EntityDefinition::s_definitions[ entityDefID ] );
	ent->currentState.id = id;
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

	//netSession->netObjectSystem->SyncObject( 1, (void*) ent );
	return ent;
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerClientState::SpawnPlayer() {
	NetMessage spawnMessage(NETMSG_SPAWN_PLAYER);
	netSession->GetHostConnection()->Send(spawnMessage);
}


