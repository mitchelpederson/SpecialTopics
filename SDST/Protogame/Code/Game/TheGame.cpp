#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Net/UDPSocket.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"


TheGame* TheGame::m_instance = nullptr;
extern bool g_isQuitting;


//----------------------------------------------------------------------------------------------------------------
void TestUDPStart( std::string const& command ) {
	//g_theGame->m_udp.start();
}


//----------------------------------------------------------------------------------------------------------------
void TestUDPSend( std::string const& command ) {
	Command comm(command);

	std::string str;
	comm.GetNextString(str);
	comm.GetNextString(str);

	NetAddress_T addr(str.c_str());

	std::string msg;
	comm.GetNextString(msg);

	//g_theGame->m_udp.send_to(addr, msg.c_str(), msg.size());
}


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

	g_theGame->netSession->AddConnection( index, NetAddress_T( addrAsString.c_str() ) );
}


//----------------------------------------------------------------------------------------------------------------
void SetMyNetID( std::string const& command ) {
	Command comm( command );
	int index;
	comm.GetFirstToken();
	comm.GetNextInt( index );
	g_theGame->netSession->SetNetIndex( index );
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


//----------------------------------------------------------------------------------------------------------------
bool OnUnreliableTest( NetMessage& message, NetConnection& sender ) {
	unsigned int count;
	unsigned int max;
	message.ReadValue<unsigned int>(&count);
	message.ReadValue<unsigned int>(&max);
	DevConsole::Printf(Rgba(100, 100, 100, 255), "Received unreliable from connection %u (%u, %u)", sender.GetConnectionIndex(), count, max );
	return true;
}


//----------------------------------------------------------------------------------------------------------------
void UnreliableTestCommand( std::string const& command ) {
	Command comm(command);
	comm.GetFirstToken();

	int connection;
	int count;
	if ( !comm.GetNextInt( connection ) ) {
		return;
	}

	if ( !comm.GetNextInt( count ) ) {
		return;
	}

	g_theGame->unreliableTestCount = 0;
	g_theGame->unreliableTestMax = count;
	g_theGame->unreliableTestConnection = connection;
}


//----------------------------------------------------------------------------------------------------------------
bool OnReliableTest( NetMessage& message, NetConnection& sender ) {
	unsigned int count;
	unsigned int max;
	message.ReadValue<unsigned int>(&count);
	message.ReadValue<unsigned int>(&max);
	DevConsole::Printf(Rgba(100, 100, 100, 255), "Received reliable from connection %u (%u, %u)", sender.GetConnectionIndex(), count, max );
	return true;
}


//----------------------------------------------------------------------------------------------------------------
void ReliableTestCommand( std::string const& command ) {
	Command comm(command);
	comm.GetFirstToken();

	int connection;
	int count;
	if ( !comm.GetNextInt( connection ) ) {
		return;
	}

	if ( !comm.GetNextInt( count ) ) {
		return;
	}

	g_theGame->reliableTestCount = 0;
	g_theGame->reliableTestMax = count;
	g_theGame->reliableTestConnection = connection;
}


//-----------------------------------------------------------------------------------------------
// Constructor, set to first wave and initial spawn
//
TheGame::TheGame() { 

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

	CommandRegistration::RegisterCommand("quit", QuitGame, "Quits the game immediately" );
	CommandRegistration::RegisterCommand("udp_send", TestUDPSend, "test UDP sending");
	CommandRegistration::RegisterCommand("udp_start", TestUDPStart, "startup UDP test socket");
	CommandRegistration::RegisterCommand("add_connection", ConnectCommand, "index ip:port");
	CommandRegistration::RegisterCommand("ping", NetPing, "index message - Send a ping to a connected user");
	CommandRegistration::RegisterCommand("add", NetAdd, "index float1 float2 - Sends an add command to another user");
	CommandRegistration::RegisterCommand("net_id", SetMyNetID, "index - Sets my NetSession's connectionIndex");
	CommandRegistration::RegisterCommand("net_set_connection_send_rate", SetConnectionSendRateCommand, "index float - Sets the send rate on a specific connection");
	CommandRegistration::RegisterCommand("unreliable_test", UnreliableTestCommand, "index count - tests unreliable messages");
	CommandRegistration::RegisterCommand("reliable_test", ReliableTestCommand, "index count - tests reliable messages");


	g_theRenderer->CreateOrGetBitmapFont("Courier");
	terrain = new SpriteSheet(g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"), IntVector2(8,8));

	m_camera = new FirstPersonCamera();
	m_camera->SetProjection(Matrix44::MakeProjection(45.f, 16.f / 9.f, 0.1f, 100.f));	
	m_camera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_camera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_cameraLight = new Light();

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

	DebugRenderSet3DCamera(m_camera);

	m_forwardRenderPath = new ForwardRenderPath(g_theRenderer);
	m_scene = new RenderSceneGraph();
	particleMaterial = g_theRenderer->GetMaterial("additive");
	m_scene->AddCamera(m_camera);

	m_skyboxTexture = g_theRenderer->CreateCubeMap("Data/Images/galaxy2.png");
	m_skyboxShader = g_theRenderer->GetShader("skybox");
	//m_camera->SetSkybox(m_skyboxTexture);

	//music = g_audioSystem->CreateOrGetSound("Data/Audio/asteroids.wav");
	//musicPlaybackID = g_audioSystem->PlaySound(music, true);
	//g_audioSystem->AddFFTToChannel(musicPlaybackID);

	netSession = new NetSession();
	netSession->AddBinding( 10084 );
	netSession->RegisterMessage( NETMSG_UNRELIABLE_TEST, "unreliable_test", OnUnreliableTest );
	netSession->RegisterMessage( NETMSG_RELIABLE_TEST, "reliable_test", OnReliableTest, NETMSG_OPTION_RELIABLE );
	netSessionWidget = new NetSessionWidget( netSession );


	unreliableTestStopwatch = new Stopwatch(m_gameClock);
	unreliableTestStopwatch->SetTimer( 1.f / 30.f );

	reliableTestStopwatch = new Stopwatch(m_gameClock);
	reliableTestStopwatch->SetTimer( 1.f / 30.f );

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

//-----------------------------------------------------------------------------------------------
// Process input relating to game logic EXCEPT pausing and time scaling
//
void TheGame::ProcessInput() {
	ProcessDebugInput();
	ProcessPlayerInput();
}


void TheGame::ProcessPlayerInput() {

}


void TheGame::ProcessDebugInput() {

	if (g_theInputSystem->WasKeyJustPressed('V')) {
		DebugRenderWireSphere(100.f, m_camera->transform.position, 1.f, Rgba(), Rgba());
	}
}


float TheGame::GetDeltaTime() {
	return m_gameClock->frame.seconds;
}

float TheGame::GetElapsedTime() {
	return m_gameClock->total.seconds;
}


//-----------------------------------------------------------------------------------------------
// Updates game logic in this order:
//	1. Get time update
//  2. Check if time scale cheat is active
//  3. Check if paused
//
void TheGame::Update() {

	netSession->ProcessIncoming();

	CheckIfPauseStateChanged();

	if (!DevConsole::GetInstance()->IsOpen()) {
		ProcessInput();
	}

	m_camera->Update();
	//m_udp.update();


	if ( unreliableTestStopwatch->CheckAndReset() && unreliableTestCount < unreliableTestMax ) {
		
		if ( g_theGame->netSession->IsValidConnectionIndex( unreliableTestConnection ) ) {
			NetMessage message( NETMSG_UNRELIABLE_TEST );
			message.WriteValue<unsigned int>(unreliableTestCount);
			message.WriteValue<unsigned int>(unreliableTestMax);
			g_theGame->netSession->GetConnection( unreliableTestConnection )->Send( message );
		}

		unreliableTestCount++;
	}


	if ( reliableTestStopwatch->CheckAndReset() && reliableTestCount < reliableTestMax ) {

		if ( g_theGame->netSession->IsValidConnectionIndex( reliableTestConnection ) ) {
			NetMessage message( NETMSG_RELIABLE_TEST );
			message.WriteValue<unsigned int>(reliableTestCount);
			message.WriteValue<unsigned int>(reliableTestMax);
			g_theGame->netSession->GetConnection( reliableTestConnection )->Send( message );
		}

		reliableTestCount++;
	}

	netSession->ProcessOutgoing();
	
}


//-----------------------------------------------------------------------------------------------
// Calls render for all game objects
//
void TheGame::Render() {
	g_theRenderer->DisableAllLights();
	// setup
	float halfWidth = Window::GetInstance()->GetWidth() / 2.f;
	float halfHeight = Window::GetInstance()->GetHeight() / 2.f;

	g_theRenderer->SetCamera(m_camera);
	m_camera->SetProjection(Matrix44::MakeProjection(40.f, halfWidth / halfHeight, 0.001f, 100.f));

	g_theRenderer->SetAmbientLight(ambientIntensity, ambientColor);
	g_theRenderer->SetSpecular(specularPower, specularAmount);

	m_forwardRenderPath->RenderSceneForCamera( m_camera, m_scene );

	g_theRenderer->SetCameraToUI();

	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(40, 0, 40, 255));
	g_theRenderer->BindMaterial(g_theRenderer->GetMaterial("ui-font"));
	g_theRenderer->DrawTextInBox2D( AABB2( 0.f, 60.f, 100.f, 100.f ), Vector2( 0.1f, 0.f ), "Unreliable UDP Test", 10.f, Rgba(30, 100, 255, 255), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_OVERRUN);
	

	std::string connectionInfo = "My binding:  ";
	connectionInfo += netSession->GetMyAddress().to_string();
	connectionInfo += "\nConnections: ";
	connectionInfo += std::to_string(netSession->GetNumberOfUsers());

	g_theRenderer->DrawTextInBox2D( AABB2( 0.f, 50.f, 100.f, 60.f ), Vector2( 0.0f, 1.f ), connectionInfo, 4.f, Rgba(), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT );


	for ( int i = 0; i < MAX_CLIENTS; i++ ) {

		if ( netSession->IsValidConnectionIndex( i ) ) {
			std::string connections = "";

			connections += "[" + std::to_string( i ) + "] ";
			connections += netSession->GetConnection(i)->GetAddressAsString();
			connections += " timeSinceLastMessage: " + std::to_string( netSession->GetTimeSinceLastMessageOnConnection(i) );
			
			g_theRenderer->DrawTextInBox2D( AABB2( 0.f, 45.f - (2.f * i), 100.f, 50.f- (2.f * i) ), Vector2( 0.f, 1.f ), connections, 2.f, Rgba(), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_WORD_WRAP );
		} 
	}

	netSessionWidget->Render();

	DebugRenderSet3DCamera(m_camera);
}


bool TheGame::IsDevModeActive() const {
	return m_devModeActive;
}

