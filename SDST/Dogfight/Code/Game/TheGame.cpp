#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerController.hpp"
#include "Game/NetController.hpp"
#include "Game/AIController.hpp"
#include "Game/MissileController.hpp"
#include "Game/EntityController.hpp"
#include "Game/PlayerInfo.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/UDPSocket.hpp"




TheGame* TheGame::m_instance = nullptr;
extern bool g_isQuitting;


//----------------------------------------------------------------------------------------------------------------
void ConnectCommand( std::string const& command ) {
	Command comm(command);
	int index;
	std::string addrAsString;

	comm.GetFirstToken();
	if ( !comm.GetNextInt( index ) ) {
		return;
	}
	addrAsString = comm.GetRemainingString();

	NetConnectionInfo_T info;
	info.addr = NetAddress_T( addrAsString.c_str() );
	info.sessionIndex = index;
	
	g_theGame->netSession->AddConnection( info );
}


//----------------------------------------------------------------------------------------------------------------
void NetAdd( std::string const& command ) {
	Command comm( command );

	int index;
	float a;
	float b;

	comm.GetFirstToken();
	if ( !comm.GetNextInt( index ) ) {
		return;
	}

	if ( !comm.GetNextFloat( a ) ) {
		return;
	}

	if ( !comm.GetNextFloat( b ) ) {
		return;
	}

	DevConsole::Printf( "Sent add." );

	NetMessage netMessage( g_theGame->netSession->GetMessageIndexForName("add") );
	netMessage.WriteValue<float>(a);
	netMessage.WriteValue<float>(b);
	NetConnection* connection = g_theGame->netSession->GetConnection( index );
	connection->Send( netMessage );

}


//----------------------------------------------------------------------------------------------------------------
void NetPing( std::string const& command ) {
	Command comm( command );
	int userIndex;
	std::string message;

	comm.GetFirstToken();
	if ( !comm.GetNextInt( userIndex ) ) {
		return;
	}

	message = comm.GetRemainingString();

	DevConsole::Printf( "Sending ping to user %u", userIndex );

	NetMessage netMessage( g_theGame->netSession->GetMessageIndexForName("ping") );
	netMessage.WriteString( message.c_str() );
	NetConnection* connection = g_theGame->netSession->GetConnection( userIndex );
	connection->Send( netMessage );
}


//----------------------------------------------------------------------------------------------------------------
void SetConnectionSendRateCommand( std::string const& command ) {
	Command comm( command );
	comm.GetFirstToken();

	int	connection;
	float tick;

	if ( !comm.GetNextInt( connection ) ) {
		return;
	}
	if ( !comm.GetNextFloat( tick ) ) {
		return;
	}

	g_theGame->netSession->SetConnectionTickRate( connection, tick );
}


//-----------------------------------------------------------------------------------------------
// Constructor, set to first wave and initial spawn
//
TheGame::TheGame() { 
	m_instance = this;
}


//-----------------------------------------------------------------------------------------------
// 
//
TheGame::~TheGame() {

}

void QuitGame( const std::string& command ) {
	command;
	g_isQuitting = true;
}

//-----------------------------------------------------------------------------------------------
// Spawns the initial asteroids
//
void TheGame::Initialize() {

	m_instance = this;

	LoadEntityDefinitions( "Data/Definitions/Entities.xml" );

	CommandRegistration::RegisterCommand("quit", QuitGame, "Quits the game immediately" );
	CommandRegistration::RegisterCommand("ping", NetPing, "index message - Send a ping to a connected user");
	CommandRegistration::RegisterCommand("add", NetAdd, "index float1 float2 - Sends an add command to another user");
	CommandRegistration::RegisterCommand("net_set_connection_send_rate", SetConnectionSendRateCommand, "index float - Sets the send rate on a specific connection");

	netSession = new NetSession();
	netSession->RegisterLeaveAndJoinCallbacks( SessionJoinCB, SessionLeaveCB );
	netSession->RegisterMessage( NETMSG_BEGIN_GAME,					"begin-game",			BeginGameCB, NETMSG_OPTION_IN_ORDER );
	netSession->RegisterMessage( NETMSG_SPAWN_PLAYER,				"spawn-player",			SpawnPlayerCB, NETMSG_OPTION_RELIABLE );
	netSession->RegisterMessage( NETMSG_SPAWN_PLAYER_REPSONSE,		"spawn-player-response", SpawnPlayerResponseCB, NETMSG_OPTION_RELIABLE );
	netSession->RegisterMessage( NETMSG_UPDATE_REMOTE_CONTROLLER,	"controller-update",	RemoteControllerUpdateCB );
	netSession->RegisterMessage( NETMSG_FIRE_MISSILE,				"fire-missile",			FireMissileCB, NETMSG_OPTION_RELIABLE );
	netSession->RegisterMessage( NETMSG_FIRE_GUN,					"fire-gun",				FireGunCB, NETMSG_OPTION_RELIABLE );
	netSession->RegisterMessage( NETMSG_CHANGE_TARGET,				"change-target",		ChangeTargetCB, NETMSG_OPTION_RELIABLE );
	netSession->RegisterMessage( NETMSG_UPDATE_PLAYER_NAME,			"player-name-change",	ChangePlayerNameCB, NETMSG_OPTION_RELIABLE );
	netSession->RegisterMessage( NETMSG_ROUND_END,					"round-end",			RoundEndCB, NETMSG_OPTION_RELIABLE );
	netSession->RegisterMessage( NETMSG_RETURN_TO_MENU,				"return-t-menu",		ReturnToMenuCB, NETMSG_OPTION_RELIABLE );
	netSessionWidget = new NetSessionWidget( netSession );

	NetObjectDef_T* entityType = new NetObjectDef_T();
	entityType->id = 1;
	entityType->sendCreateCB = SendEntityCreate;
	entityType->recvCreateCB = RecvEntityCreate;
	entityType->sendDestroyCB = SendEntityDestroy;
	entityType->recvDestroyCB = RecvEntityDestroy;
	entityType->getSnapshotCB = GetEntitySnapshot;
	entityType->sendSnapshotCB = SendEntitySnapshot;
	entityType->recvSnapshotCB = RecvEntitySnapshot;
	entityType->applySnapshotCB = ApplyEntitySnapshot;
	netSession->netObjectSystem->RegisterObjectType( entityType );

	NetObjectDef_T* playerInfoType = new NetObjectDef_T();
	playerInfoType->id = 2;
	playerInfoType->sendCreateCB = SendPlayerInfoCreate;
	playerInfoType->recvCreateCB = RecvPlayerInfoCreate;
	playerInfoType->sendDestroyCB = SendPlayerInfoDestroy;
	playerInfoType->recvDestroyCB = RecvPlayerInfoDestroy;
	playerInfoType->getSnapshotCB = GetPlayerInfoSnapshot;
	playerInfoType->sendSnapshotCB = SendPlayerInfoSnapshot;
	playerInfoType->recvSnapshotCB = RecvPlayerInfoSnapshot;
	playerInfoType->applySnapshotCB = ApplyPlayerInfoSnapshot;
	netSession->netObjectSystem->RegisterObjectType( playerInfoType );


	g_theRenderer->CreateOrGetBitmapFont("Courier");
	g_theRenderer->SetFog(0.f, 0.f, Rgba());

	m_gameClock = new Clock(g_masterClock);

	MeshBuilder builder;
	m_sphereMesh = new Mesh();
	builder.BuildSphere(m_sphereMesh, Vector3(), 0.5f, 20, 20);
	m_sphereMesh->SetDrawPrimitive(TRIANGLES);
	m_cubeMesh = new Mesh();
	builder.BuildCube(m_cubeMesh, Vector3::ZERO, Vector3(0.5f, 0.5f, 0.5f));
	m_quadMesh = new Mesh();
	builder.BuildQuad(m_quadMesh, Vector3::ZERO, Vector3::UP, Vector3::RIGHT, Rgba());
	m_basisMesh = new Mesh();
	builder.BuildBasis(m_basisMesh);

	//m_jetMesh = g_theRenderer->CreateOrGetMesh( "Data/Models/EurofighterSimplified/Eurofighter.obj" );

	LoadState* state = new LoadState();
	m_currentStatePtr = state;
}


//-----------------------------------------------------------------------------------------------
// Removes the given bullet and packs the array
//
void TheGame::CheckIfPauseStateChanged() {
	if (!DevConsole::GetInstance()->IsOpen() && g_theInputSystem->WasKeyJustPressed('P') && m_isPaused == false) {
		m_gameClock->SetPaused(true);
	}
	else {
		m_gameClock->SetPaused(false);
	}
}


//----------------------------------------------------------------------------------------------------------------
float TheGame::GetDeltaTime() {
	return m_gameClock->frame.seconds;
}


//----------------------------------------------------------------------------------------------------------------
float TheGame::GetElapsedTime() {
	return m_gameClock->total.seconds;
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update() {

	netSession->ProcessIncoming();

	CheckIfPauseStateChanged();
	if ( m_nextState != STATE_NONE && m_currentStatePtr->IsReadyToExit() ) {
		GoToNextState();
	}

	m_currentStatePtr->Update();

	netSession->ProcessOutgoing();
	netSessionWidget->Update();
}



//----------------------------------------------------------------------------------------------------------------
void TheGame::BeginTransitionToState( eGameState next ) {
	m_nextState = next;
	m_currentStatePtr->OnBeginExit();
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::GoToNextState() {

	delete m_currentStatePtr;

	if (m_nextState == STATE_MENU_MAIN) {
		m_currentStatePtr = new MenuState();
	}
	else if (m_nextState == STATE_MENU_SETUP) {
		m_currentStatePtr = new SetupState();
	}
	else if (m_nextState == STATE_MENU_HOSTING) {
		m_currentStatePtr = new MenuHostState();
	}
	else if (m_nextState == STATE_MENU_JOINING) {
		m_currentStatePtr = new MenuJoinState();
	}
	else if (m_nextState == STATE_PLAY_HOST) {
		m_currentStatePtr = new MultiplayerHostState();
	}
	else if (m_nextState == STATE_PLAY_CLIENT) {
		m_currentStatePtr = new MultiplayerClientState();
	}

	m_currentState = m_nextState;
	m_nextState = STATE_NONE;

	m_currentStatePtr->OnEnter();
}


//-----------------------------------------------------------------------------------------------
// Calls render for all game objects
//
void TheGame::Render() {
	
	/*g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));
	g_theRenderer->DrawTextInBox2D( AABB2( 0.f, 60.f, 100.f, 100.f ), Vector2( 0.1f, 0.f ), "NetObjectSystem Test", 10.f, Rgba(30, 100, 255, 255), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_OVERRUN);
	*/

	m_currentStatePtr->Render();
	netSessionWidget->Render();
}

//----------------------------------------------------------------------------------------------------------------
bool TheGame::IsDevModeActive() const {
	return m_devModeActive;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::AddConnectedPlayer( PlayerInfo* playerInfo ) {
	if ( netSession->AmIHost() ) {
		netSession->netObjectSystem->SyncObject( 2, playerInfo );
	}
	m_connectedPlayers[playerInfo->GetConnectionID()] = playerInfo;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::RemoveConnectedPlayer( uint8_t connID ) {
	if ( netSession->AmIHost() ) {
		netSession->netObjectSystem->UnsyncObject( m_connectedPlayers[connID] );
	}
	delete m_connectedPlayers[connID];
	m_connectedPlayers.erase( connID );
}


//----------------------------------------------------------------------------------------------------------------
PlayerInfo* TheGame::GetPlayerInfo( uint8_t connID ) {
	std::map< uint8_t, PlayerInfo* >::iterator it = m_connectedPlayers.find(connID);
	if (it != m_connectedPlayers.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}	
}


//----------------------------------------------------------------------------------------------------------------
PlayerInfo* TheGame::GetMyPlayerInfo() {
	return m_connectedPlayers[ netSession->GetMyConnectionIndex() ];
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::UpdateMyCallsign( const std::string& name ) {
	PlayerInfo* myInfo = m_connectedPlayers[ netSession->GetMyConnectionIndex() ];
	myInfo->SetName(name);

	if ( netSession->AmIHost() ) {

		NetMessage msg(NETMSG_UPDATE_PLAYER_NAME);
		msg.WriteValue<uint8_t>( myInfo->GetConnectionID() );
		msg.WriteString( myInfo->GetName().c_str() );

		netSession->SendToAllOtherConnections( msg );
	}

	else {


		NetMessage msg(NETMSG_UPDATE_PLAYER_NAME);
		msg.WriteValue<uint8_t>( myInfo->GetConnectionID() );
		msg.WriteString( myInfo->GetName().c_str() );
		netSession->GetHostConnection()->Send( msg );
		Logger::PrintTaggedf("DevConsole", "Sent name change request %s", myInfo->GetName().c_str());
	}
}



//----------------------------------------------------------------------------------------------------------------
// NET CALLBACKS
//----------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
bool SessionJoinCB( void* data ) {


	if ( g_theGame->netSession->AmIHost() ) {
		NetConnection* conn = (NetConnection*) data;
		PlayerInfo* playerInfo = new PlayerInfo(conn->GetConnectionIndex());
		playerInfo->SetName(conn->GetID());

		g_theGame->AddConnectedPlayer( playerInfo );
		//TheGame::GetMultiplayerState()->localPlayer->connectionID = g_theGame->netSession->GetMyConnectionIndex();
	}
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool SessionLeaveCB( void* data ) {
	NetConnection* conn = (NetConnection*) data;
	g_theGame->RemoveConnectedPlayer( conn->GetConnectionIndex() );
	return true;
}


//----------------------------------------------------------------------------------------------------------------
// Only called as a client
bool BeginGameCB( NetMessage& message, NetConnection& sender ) {
	g_theGame->BeginTransitionToState( STATE_PLAY_CLIENT );
	return true;
}


//----------------------------------------------------------------------------------------------------------------
// Only called by a host
bool SpawnPlayerCB( NetMessage& message, NetConnection& sender ) {
	MultiplayerHostState* mps = g_theGame->GetMultiplayerStateAsHost();

	if ( mps->FindPlayerByConnection( sender.GetConnectionIndex() ) == nullptr ) {

		Entity* p;
		
		if ( g_theGame->netSession->GetMyConnectionIndex() == sender.GetConnectionIndex() ) {
			p = mps->CreateEntity( 0, new PlayerController(), sender.GetConnectionIndex() );
		} else {
			p = mps->CreateEntity( 0, new NetController(), sender.GetConnectionIndex() );
		}
		p->Spawn();
		
		NetMessage response(NETMSG_SPAWN_PLAYER_REPSONSE);
		response.WriteValue<int>( p->currentState.id );
		sender.Send( response );

		return true;
	} 
	else {
		return false;
	}
}


//----------------------------------------------------------------------------------------------------------------
// Only called as client
bool SpawnPlayerResponseCB( NetMessage& message, NetConnection& sender ) {
	//g_theGame->localPlayer = g_theGame->FindPlayerByConnection( g_theGame->netSession->GetMyConnectionIndex() )->controller;
	//g_theGame->localPlayer->entity->followCamera = g_theGame->m_camera;
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool RemoteControllerUpdateCB( NetMessage& message, NetConnection& sender ) {

	int controllerID;
	float throttle;
	float rollAxis;
	float pitchAxis;
	float yawAxis;
	bool isFireMissilePressed;
	bool isFireGunPressed;

	message.ReadValue<int>( &controllerID );
	message.ReadValue<float>( &throttle );
	message.ReadValue<float>( &rollAxis );
	message.ReadValue<float>( &pitchAxis );
	message.ReadValue<float>( &yawAxis );
	message.ReadValue<bool>( &isFireMissilePressed );
	message.ReadValue<bool>( &isFireGunPressed );
	Entity* entity = TheGame::GetMultiplayerState()->FindPlayerByConnection( controllerID );

	if (entity != nullptr) {
		entity->controller->throttle = throttle;
		entity->controller->rollAxis = rollAxis;
		entity->controller->pitchAxis = pitchAxis;
		entity->controller->yawAxis = yawAxis;
		entity->controller->isFireMissilePressed = isFireMissilePressed;
		entity->controller->isFireGunPressed = isFireGunPressed;
	}
	
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool ChangePlayerNameCB( NetMessage& message, NetConnection& sender ) {
	NetSession* session = g_theGame->netSession;
	uint8_t connID;
	char nameBuffer[20];

	message.ReadValue<uint8_t>( &connID );
	size_t charCount = message.ReadString( nameBuffer, 19 );
	std::string newName( nameBuffer, charCount ); // truncate unused characters

	PlayerInfo* player = g_theGame->GetPlayerInfo( connID );
	player->SetName(newName);

	// If a client requested this name change, forward that to all connected clients
	if ( session->AmIHost() ) {
		Logger::PrintTaggedf("Net", "Received a player name change from client %s", newName.c_str());
		session->SendToAllOtherConnections( message );
	} 

	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool FireMissileCB( NetMessage& message, NetConnection& sender ) {

	int targetID;
	Vector3 pos;
	Vector3 orientation;
	Vector3 vel;
	message.ReadValue<int>( &targetID );

	Entity* firingEntity = TheGame::GetMultiplayerState()->FindPlayerByConnection( sender.GetConnectionIndex() );
	firingEntity->FireMissile();
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool FireGunCB( NetMessage& message, NetConnection& sender ) {

	Entity* firingEntity = TheGame::GetMultiplayerState()->FindPlayerByConnection( sender.GetConnectionIndex() );
	firingEntity->FireMachineGun();
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool ChangeTargetCB( NetMessage& message, NetConnection& sender ) {
	Entity* entity = TheGame::GetMultiplayerState()->FindPlayerByConnection( sender.GetConnectionIndex() );

	if (entity != nullptr) {
		entity->SwitchLockedTarget();
		return true;
	} else {
		return false;
	}

}


//----------------------------------------------------------------------------------------------------------------
bool RoundEndCB( NetMessage& message, NetConnection& sender ) {
	uint8_t winnerConnID;
	message.ReadValue<uint8_t>( &winnerConnID );
	if ( winnerConnID == g_theGame->GetMyPlayerInfo()->GetConnectionID() ) {
		g_theGame->GetMultiplayerStateAsClient()->currentSubstate = CLIENT_STATE_VICTORY;
	} else {
		g_theGame->GetMultiplayerStateAsClient()->currentSubstate = CLIENT_STATE_LOSS;
	}
	g_theGame->GetMultiplayerState()->m_winnerInfo = g_theGame->GetPlayerInfo( winnerConnID );
	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool ReturnToMenuCB( NetMessage& message, NetConnection& sender ) {
	g_theGame->BeginTransitionToState(STATE_MENU_JOINING);
	return true;
}


//----------------------------------------------------------------------------------------------------------------
void SendEntityCreate( NetMessage* msg, void* obj ) {

	Entity* ent = (Entity*) obj;

	msg->WriteValue<int>( ent->def.GetID() );
	msg->WriteValue<uint8_t>( ent->controller->connectionID );
	ent->currentState.WriteToBytePacker( msg );
}


//----------------------------------------------------------------------------------------------------------------
void* RecvEntityCreate( NetMessage* msg ) {
	
	EntitySnapshot_T entityState;
	int defID;
	uint8_t connectionID;

	msg->ReadValue<int>( &defID );
	msg->ReadValue<uint8_t>( &connectionID );
	entityState.ReadFromBytePacker(msg);


	EntityController* controller;
	if ( connectionID == g_theGame->netSession->GetMyConnectionIndex() ) {
		controller = new PlayerController();
	} else {
		controller = new NetController();
	}
	controller->connectionID = connectionID;

	Entity* ent = TheGame::GetMultiplayerStateAsClient()->CreateEntityFromNet( entityState.id, defID, controller );

	ent->currentState = entityState;

	TheGame::GetMultiplayerStateAsClient()->entities[entityState.id] = ent;
	ent->Spawn();
	return ent;
}


//----------------------------------------------------------------------------------------------------------------
void SendEntityDestroy( NetMessage* msg, void* obj ) {
	// nothing
}


//----------------------------------------------------------------------------------------------------------------
void RecvEntityDestroy( NetMessage* msg, void* obj ) {
	Entity* ent = (Entity*) obj;

	ent->Kill(-1);
	//g_theGame->DestroyEntity( ent );
}


//----------------------------------------------------------------------------------------------------------------
void GetEntitySnapshot( void*& snapshot, void* obj ) {
	EntitySnapshot_T* ss = new EntitySnapshot_T();
	Entity* entity = (Entity*) obj;
	entity->currentState.timestamp = g_theGame->m_gameClock->GetCurrentTimeSeconds();

	*ss = entity->currentState;
	snapshot = ss;
}


//----------------------------------------------------------------------------------------------------------------
void SendEntitySnapshot( NetMessage* msg, void* snapshot ) {
	EntitySnapshot_T* ss = (EntitySnapshot_T*) snapshot;

	ss->WriteToBytePacker(msg);
}


//----------------------------------------------------------------------------------------------------------------
void RecvEntitySnapshot( NetMessage* msg, void* snapshot ) {
	EntitySnapshot_T* ss = (EntitySnapshot_T*) snapshot;

	ss->ReadFromBytePacker(msg);
}


//----------------------------------------------------------------------------------------------------------------
void ApplyEntitySnapshot( void* snapshot, void* obj, float snapshotAge ) {
	EntitySnapshot_T* ss = (EntitySnapshot_T*) snapshot;
	Entity* entity = (Entity*) obj;
	entity->UpdateLastReceivedSnapshot( ss, g_theGame->m_gameClock->GetCurrentTimeSeconds() - ss->timestamp );
}


//----------------------------------------------------------------------------------------------------------------
void SendPlayerInfoCreate( NetMessage* msg, void* obj ) {
	PlayerInfo* playerInfo = (PlayerInfo*) obj;
	msg->WriteValue<uint8_t>( playerInfo->GetConnectionID() );
	msg->WriteString( playerInfo->GetName().c_str() );
}


//----------------------------------------------------------------------------------------------------------------
void* RecvPlayerInfoCreate( NetMessage* msg ) {
	uint8_t connID;
	char name[9];

	msg->ReadValue<uint8_t>( &connID );
	size_t readChars = msg->ReadString( name, 9 );
	std::string strName( name, readChars ); // truncate string based on num written chars

	PlayerInfo* playerInfo = new PlayerInfo(connID);
	playerInfo->SetName(strName);

	g_theGame->AddConnectedPlayer( playerInfo );

	return playerInfo;
}


//----------------------------------------------------------------------------------------------------------------
void SendPlayerInfoDestroy( NetMessage* msg, void* obj ) {
	// do nothing
}


//----------------------------------------------------------------------------------------------------------------
void RecvPlayerInfoDestroy( NetMessage* msg, void* obj ) {
	PlayerInfo* playerInfo = (PlayerInfo*) obj;
	g_theGame->RemoveConnectedPlayer( playerInfo->GetConnectionID() );
}


//----------------------------------------------------------------------------------------------------------------
void GetPlayerInfoSnapshot( void*& snapshot, void* obj ) {
	PlayerInfoSnapshot_T* ss = new PlayerInfoSnapshot_T();
	PlayerInfo* playerInfo = (PlayerInfo*) obj;

	ss->kills = playerInfo->GetKills();
	ss->gunHits = playerInfo->GetGunHits();
	ss->missileHits = playerInfo->GetMissileHits();
	ss->score = playerInfo->GetScore();

	snapshot = ss;
}


//----------------------------------------------------------------------------------------------------------------
void SendPlayerInfoSnapshot( NetMessage* msg, void* snapshot ) {
	PlayerInfoSnapshot_T* ss = (PlayerInfoSnapshot_T*) snapshot;

	msg->WriteValue<int>( ss->score );
	msg->WriteValue<int>( ss->kills );
	msg->WriteValue<int>( ss->gunHits );
	msg->WriteValue<int>( ss->missileHits );
}


//----------------------------------------------------------------------------------------------------------------
void RecvPlayerInfoSnapshot( NetMessage* msg, void* snapshot ) {
	int score;
	int kills;
	int gunHits;
	int missileHits;

	msg->ReadValue<int>( &score  );
	msg->ReadValue<int>( &kills  );
	msg->ReadValue<int>( &gunHits  );
	msg->ReadValue<int>( &missileHits  );

	PlayerInfoSnapshot_T* ss = (PlayerInfoSnapshot_T*) snapshot;
	ss->score = score;
	ss->kills = kills;
	ss->gunHits = gunHits;
	ss->missileHits = missileHits;
}


//----------------------------------------------------------------------------------------------------------------
void ApplyPlayerInfoSnapshot( void* snapshot, void* obj, float snapshotAge ) {
	PlayerInfoSnapshot_T* ss = (PlayerInfoSnapshot_T*) snapshot;
	PlayerInfo* playerInfo = (PlayerInfo*) obj;
	playerInfo->UpdateFromSnapshot( ss );
}


//----------------------------------------------------------------------------------------------------------------
// XML READS
//----------------------------------------------------------------------------------------------------------------
void TheGame::LoadEntityDefinitions( std::string path ) {

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile( path.c_str() );

	const tinyxml2::XMLElement* root = doc->FirstChildElement( "entities" );
	const tinyxml2::XMLElement* entityDef = root->FirstChildElement( "entity" );


	while ( entityDef != nullptr ) {
		new EntityDefinition( *entityDef );
		entityDef = entityDef->NextSiblingElement( "entity" );
	}

	delete doc;
}


MultiplayerState* TheGame::GetMultiplayerState() {
	return dynamic_cast< MultiplayerState* >( TheGame::GetInstance()->m_currentStatePtr );
}


MultiplayerClientState* TheGame::GetMultiplayerStateAsClient() {
	return dynamic_cast< MultiplayerClientState* >( TheGame::GetInstance()->m_currentStatePtr );
}


//----------------------------------------------------------------------------------------------------------------
MultiplayerHostState* TheGame::GetMultiplayerStateAsHost() {
	return dynamic_cast< MultiplayerHostState* >( TheGame::GetInstance()->m_currentStatePtr );
}

TheGame* TheGame::GetInstance() {
	return m_instance;
}