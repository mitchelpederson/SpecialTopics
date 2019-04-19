#include "Game/GameState/MultiplayerState.hpp"
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Profiler/Profiler.hpp"


//----------------------------------------------------------------------------------------------------------------
MultiplayerState::MultiplayerState()
	: m_sceneClock( g_theGame->m_gameClock )
{
	netSession = g_theGame->netSession;
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerState::Initialize() {

	delete g_theGame->m_gameClock;
	g_theGame->m_gameClock = new Clock( g_theGame->netSession->m_sessionClock );
	DebugRenderSetClock( g_theGame->m_gameClock );
	m_matchStartTime = g_theGame->m_gameClock->GetCurrentTimeSeconds();

	m_camera = new FirstPersonCamera();
	m_camera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_camera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_camera->SetBloomTarget(g_theRenderer->GetDefaultBloomColorTarget());
	m_camera->transform.position = Vector3( 0.f, 0.f, -50.f );
	m_camera->SetBloomEnabled( true );
	m_cameraLight = new Light();
	m_cameraLight->SetAsDirectionalLight( Vector3( 0.f, 0.f, 0.f ), Vector3( -0.7f, -0.7f, 0.f ).GetNormalized(), Rgba(200, 210, 185, 255), 0.7, 1.f );
	DebugRenderSet3DCamera(m_camera);

	m_forwardRenderPath = new ForwardRenderPath(g_theRenderer);
	m_scene = new RenderSceneGraph();
	m_scene->AddCamera(m_camera);
	m_scene->AddLight( m_cameraLight );

	m_skyboxTexture = g_theRenderer->CreateCubeMap("Data/Images/skybox.png");
	m_skyboxShader = g_theRenderer->GetShader("skybox");
	m_camera->SetSkybox(m_skyboxTexture);

	hud = new PlayerHUD();
	m_terrain = new Terrain( m_camera );
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerState::DestroyEntity( Entity* entity ) {
	std::map< int, Entity* >::iterator entityIt = entities.find( entity->currentState.id );
	std::map< int, EntityController* >::iterator controllerIt = controllers.find( entity->currentState.id );

	controllers.erase( controllerIt );

	netSession->netObjectSystem->UnsyncObject( (void*) entity );

	if ( entity->controller == localPlayer ) {
		localPlayer = nullptr;
		hud->SetPlayerController( nullptr );
	}

	delete entity->controller;

	entities.erase( entityIt );
	delete entity;
}


//----------------------------------------------------------------------------------------------------------------
Entity* MultiplayerState::GetEntityByID( int entityToFind ) {
	std::map< int, Entity* >::iterator it = entities.find( entityToFind );
	if ( it != entities.end() ) {
		return it->second;
	} else {
		return nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
Entity* MultiplayerState::FindPlayerByConnection( uint8_t connectionIndex ) {
	std::map< int, Entity* >::iterator it = entities.begin();
	while ( it != entities.end() ) {

		if ( it->second->controller->connectionID == connectionIndex ) {
			return it->second;
		}

		it++;
	}

	return nullptr;
}


//----------------------------------------------------------------------------------------------------------------
bool MultiplayerState::DoesEntityExist( int entityToFind ) {
	Entity* result = GetEntityByID( entityToFind );
	if ( result == nullptr ) {
		return false;
	}

	if ( result->IsAlive() ) {
		return true;
	} else {
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------------
int MultiplayerState::GetNextEntityToLockFromID( int lockingID, int prevID ) {
	bool looping = true;
	bool hasWrapped = false;

	std::map< int, Entity* >::iterator it = entities.find( prevID );
	if ( it == entities.end() ) {
		it = entities.begin();
	}

	while ( looping ) {
		it++;
		if (it == entities.end() && hasWrapped == false) {
			it = entities.begin();
			hasWrapped = true;
		} else if ( it == entities.end() && hasWrapped == true ) {
			looping = false;
			return -1;
		}

		if ( it->first != lockingID && it->first != prevID && it->second->def.CanReceiveLock() ) {
			return it->first;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerState::UpdateEntitiesAndControllers() {
	std::map< int, EntityController* >::iterator controllerIterator = controllers.begin();
	while ( controllerIterator != controllers.end() ) {
		if ( controllerIterator->second->entity->IsAlive() ) {
			controllerIterator->second->Update();
		}
		controllerIterator++;
	}

	std::map< int, Entity* >::iterator entityIterator = entities.begin();
	while ( entityIterator != entities.end() ) {
		if ( entityIterator->second->IsAlive() ) {
			entityIterator->second->Update();
		}
		entityIterator++;
	}

	for ( int i = 0; i < m_orphanedParticleEmitters.size(); i++ ) {
		m_orphanedParticleEmitters[i]->Update( m_orphanedParticleEmitters[i] );
	}
}

//----------------------------------------------------------------------------------------------------------------
void MultiplayerState::CheckEntityCollisions() {
	std::map< int, Entity* >::iterator firstEntityIt = entities.begin();

	while ( firstEntityIt != entities.end() ) {
		Entity* outerEntity = firstEntityIt->second;

		if ( outerEntity->IsAlive() ) {

			std::map< int, Entity* >::iterator secondEntityIt = entities.begin();
			while ( secondEntityIt != entities.end() ) {
				Entity* innerEntity = secondEntityIt->second;
				if ( innerEntity->IsAlive() && innerEntity != outerEntity && !AreOwnedBySamePlayer(innerEntity, outerEntity) ) {

					Vector3 displacement = outerEntity->GetPosition() - innerEntity->GetPosition();
					float distance = displacement.GetLength();
					if ( distance <= outerEntity->def.GetPhysicalRadius() + innerEntity->def.GetPhysicalRadius() ) {
						
						// Give kills + points for the hit, if i'm the host and 
						// one of the entities was a weapon
						if ( g_theGame->netSession->AmIHost() ) {

							PlayerInfo* innerInfo = g_theGame->GetPlayerInfo(innerEntity->controller->connectionID);
							PlayerInfo* outerInfo = g_theGame->GetPlayerInfo(outerEntity->controller->connectionID);
							
							if ( outerEntity->IsWeapon() && outerInfo != nullptr) {
								if ( outerEntity->def.GetID() == 10 ) {
									innerEntity->Damage( 45.f, outerInfo->GetConnectionID(), true );
								} else {
									innerEntity->Damage( 3.f, outerInfo->GetConnectionID(), false );
								}
								outerEntity->Kill(-1);
							}

							if ( innerEntity->IsWeapon() && innerInfo != nullptr ) {
								if ( innerEntity->def.GetID() == 10 ) {
									outerEntity->Damage( 45.f, innerInfo->GetConnectionID(), true );
								} else {
									outerEntity->Damage( 3.f, innerInfo->GetConnectionID(), false );
								}
								innerEntity->Kill(-1);
							} 
						}
					}
				}

				secondEntityIt++;
			}

		}
		firstEntityIt++;
	}
}


//----------------------------------------------------------------------------------------------------------------
bool MultiplayerState::AreOwnedBySamePlayer( Entity* first, Entity* second ) {
	if ( first->controller->connectionID != -1 && second->controller->connectionID != -1 ) {
		if ( first->controller->connectionID == second->controller->connectionID ) {
			return true;
		}
	}
	return false;
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerState::ClearDeadEntities() {
	std::map< int, Entity* >::iterator entityIt = entities.begin();
	while ( entityIt != entities.end() ) {
		if ( entityIt->second->IsAlive() == false ) {
			DestroyEntity( entityIt->second );
			entityIt = entities.begin();
		}
		entityIt++;
	}

	for ( int i = 0; i < m_orphanedParticleEmitters.size(); i++ ) {
		if ( m_orphanedParticleEmitters[i]->IsSafeToDestroy() ) {
			m_scene->RemoveParticleEmitter(m_orphanedParticleEmitters[i]);
			delete m_orphanedParticleEmitters[i];
			m_orphanedParticleEmitters[i] = m_orphanedParticleEmitters[m_orphanedParticleEmitters.size() - 1];
			m_orphanedParticleEmitters.pop_back();
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void MultiplayerState::DrawScaleGridAroundPlayer() {
	Renderer* r = g_theRenderer;
	Vector3 playerPos = localPlayer->entity->GetPosition();

	int kmX = floorf(playerPos.x / 1000.f);
	int kmY = floorf(playerPos.y / 1000.f);
	int kmZ = floorf(playerPos.z / 1000.f);

	// lines along Y
	for ( int x = -4; x < 5; x++ ) {
		for ( int z = -4; z < 5; z++ ) {
			int opacity = Interpolate( 20, 255, ( (float) (9 - (abs(x) + abs(z))) / 9.f));
			Vector3 bottom( (kmX + x) * 1000, (kmY - 4) * 1000, (kmZ + z) * 1000 );
			Vector3 top( (kmX + x) * 1000, (kmY + 4) * 1000, (kmZ + z) * 1000 );
			DebugRenderLineSegment( 0.f, bottom, Rgba(255, 255, 255, 255), top, Rgba(255, 255, 255, 255), Rgba(0, 255, 0, opacity), Rgba(0, 255, 0, opacity) );
		}
	}

	// lines along Z
	for ( int x = -4; x < 5; x++ ) {
		for ( int y = -4; y < 5; y++ ) {
			int opacity = Interpolate( 20, 255, ( (float) (9 - (abs(x) + abs(y))) / 9.f));
			Vector3 back( (kmX + x) * 1000, (kmY + y) * 1000, (kmZ - 4) * 1000 );
			Vector3 front( (kmX + x) * 1000, (kmY + y) * 1000, (kmZ + 4) * 1000 );
			DebugRenderLineSegment( 0.f, back, Rgba(255, 255, 255, 255), front, Rgba(255, 255, 255, 255), Rgba(0, 0, 255, opacity), Rgba(0, 0, 255, opacity) );
		}
	}


	// lines along X
	for ( int z = -4; z < 5; z++ ) {
		for ( int y = -4; y < 5; y++ ) {
			int opacity = Interpolate( 20, 255, ( (float) (9 - (abs(y) + abs(z))) / 9.f));
			Vector3 left( (kmX - 4) * 1000, (kmY + y) * 1000, (kmZ + z) * 1000 );
			Vector3 right( (kmX + 4) * 1000, (kmY + y) * 1000, (kmZ + z) * 1000 );
			DebugRenderLineSegment( 0.f, left, Rgba(255, 0, 0, 255), right, Rgba(255, 0, 0, 255), Rgba(255, 0, 0, opacity), Rgba(255, 0, 0, opacity) );
		}
	}


	Vector3 playerAccel = localPlayer->entity->currentState.acceleration + playerPos;
	Vector3 playerVel = localPlayer->entity->currentState.velocity + playerPos;
	//playerAccel *= 100.f;
	DebugRenderLineSegment( 0.f, playerPos, Rgba(), playerAccel, Rgba() );
	DebugRenderLineSegment( 0.f, playerPos, Rgba(), playerVel, Rgba(), Rgba(255, 0, 0, 255), Rgba(255, 0, 0, 255) );
}


//----------------------------------------------------------------------------------------------------------------
float MultiplayerState::GetTimeSinceRoundStart() const {
	return g_theGame->GetElapsedTime() - m_matchStartTime;
}


//----------------------------------------------------------------------------------------------------------------
bool MultiplayerState::IsPointBelowTerrain( const Vector3& point ) {
	return m_terrain->IsPointBelowTerrain( point );
}