#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Net/NetSession.hpp"
#include "Engine/DevConsole/NetSessionWidget.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

#include "Game/Entity.hpp"
#include "Game/EntityController.hpp"
#include "Game/PlayerInfo.hpp"
#include "Game/GameState/MultiplayerHostState.hpp"
#include "Game/GameState/MultiplayerClientState.hpp"
#include "Game/GameState/MenuHostState.hpp"
#include "Game/GameState/MenuJoinState.hpp"
#include "Game/GameState/LoadState.hpp"
#include "Game/GameState/MenuState.hpp"
#include "Game/GameState/SetupState.hpp"

#include <vector>
#include <map>

enum DevModeShader {
	DEV_SHADER_NONE,
	DEV_SHADER_VERTEX_NORMALS,
	DEV_SHADER_SURFACE_NORMALS,
	DEV_SHADER_WORLD_NORMALS,
	DEV_SHADER_TANGENTS,
	DEV_SHADER_BITANGENTS,
	DEV_SHADER_COLOR,
	DEV_SHADER_VERTEX_TANGENTS,
	DEV_SHADER_VERTEX_BITANGENTS,
	NUM_DEV_SHADERS
};


enum eNetGameMessage {
	NETMSG_GAME_TEST = NETMSG_CORE_COUNT,

	NETMSG_BEGIN_GAME = 40,
	NETMSG_UPDATE_PLAYER_NAME = 41,
	NETMSG_ROUND_END = 42,
	NETMSG_RETURN_TO_MENU = 43,

	NETMSG_SPAWN_PLAYER = 50,
	NETMSG_SPAWN_PLAYER_REPSONSE = 51,
	NETMSG_UPDATE_REMOTE_CONTROLLER = 52,

	NETMSG_FIRE_MISSILE = 70, // unused
	NETMSG_FIRE_GUN = 71, // unused
	NETMSG_CHANGE_TARGET = 72,


	NETMSG_UNRELIABLE_TEST = 128,
	NETMSG_RELIABLE_TEST = 129,
	NETMSG_IN_ORDER_TEST = 130
};


enum eGameState {
	STATE_NONE,
	STATE_LOAD,
	STATE_MENU_MAIN,
	STATE_MENU_HOSTING,
	STATE_MENU_JOINING,
	STATE_MENU_SETUP,
	STATE_PLAY_HOST,
	STATE_PLAY_CLIENT,
	STATE_PLAY_SOLO
};


//-----------------------------------------------------------------------------------------------
// TheGame class created by Mitchel Pederson
// 
class TheGame {

public:
	TheGame();
	~TheGame();

	void Initialize();

	bool IsDevModeActive() const;
	void Update();
	void Render();
	void BeginTransitionToState( eGameState next );

	float GetDeltaTime();
	float GetElapsedTime();

	static MultiplayerState* GetMultiplayerState();
	static MultiplayerHostState* GetMultiplayerStateAsHost();
	static MultiplayerClientState* GetMultiplayerStateAsClient();
	static TheGame* GetInstance();

	void AddConnectedPlayer( PlayerInfo* playerInfo );
	void RemoveConnectedPlayer( uint8_t connID );
	PlayerInfo* GetPlayerInfo( uint8_t connID );
	PlayerInfo* GetMyPlayerInfo();
	void UpdateMyCallsign( const std::string& name );
	
public:
	Clock* m_gameClock;
	NetSession* netSession = nullptr;
	NetSessionWidget* netSessionWidget = nullptr;

	
private:
	void LoadEntityDefinitions( std::string path );
	void GoToNextState();


private:

	eGameState m_currentState = STATE_LOAD;
	eGameState m_nextState = STATE_MENU_MAIN;
	GameState* m_currentStatePtr;

	// Constant stuff
	void CheckIfPauseStateChanged();

	static TheGame* m_instance;
	std::map< uint8_t, PlayerInfo* > m_connectedPlayers;

	bool m_devModeActive;
	DevModeShader m_devModeShader = DEV_SHADER_NONE;

	bool m_isPaused;

	Mesh* m_sphereMesh;
	Mesh* m_quadMesh;
	Mesh* m_cubeMesh;
	Mesh* m_basisMesh;
	Mesh* m_shipMesh;
	Mesh* m_jetMesh;
	Renderable* m_jetRenderable;


	std::string debugShaderName = "";

	Light* m_cameraLight = nullptr;

	std::vector<Light*> m_pointLights;

};


bool SessionJoinCB( void* data );
bool SessionLeaveCB( void* data );

bool BeginGameCB( NetMessage& message, NetConnection& sender );
bool SpawnPlayerCB( NetMessage& message, NetConnection& sender );
bool SpawnPlayerResponseCB( NetMessage& message, NetConnection& sender );
bool RemoteControllerUpdateCB( NetMessage& message, NetConnection& sender );
bool FireMissileCB( NetMessage& message, NetConnection& sender );
bool FireGunCB( NetMessage& message, NetConnection& sender );
bool ChangeTargetCB( NetMessage& message, NetConnection& sender );
bool ChangePlayerNameCB( NetMessage& message, NetConnection& sender );
bool RoundEndCB( NetMessage& message, NetConnection& sender );
bool ReturnToMenuCB( NetMessage& message, NetConnection& sender );