#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/States/GameState.hpp"
#include "Game/States/LoadState.hpp"
#include "Game/States/PlayState.hpp"
#include "Game/States/MenuState.hpp"
#include "Game/States/SetupState.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Sprites/IsoSprite.hpp"
#include "Engine/Renderer/Sprites/IsoSpriteAnimDef.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Audio/AudioSystem.hpp"


TheGame* TheGame::m_instance = nullptr;
extern bool g_isQuitting;


//----------------------------------------------------------------------------------------------------------------
// Constructor, set to first wave and initial spawn
TheGame::TheGame() { 

}


//----------------------------------------------------------------------------------------------------------------
TheGame::~TheGame() {

}


//----------------------------------------------------------------------------------------------------------------
void QuitGame( const std::string& command ) {
	g_isQuitting = true;
}


//----------------------------------------------------------------------------------------------------------------
// Spawns the initial asteroids
void TheGame::Initialize() {

	CommandRegistration::RegisterCommand("quit", QuitGame);

	terrain = new SpriteSheet(g_theRenderer->CreateOrGetTexture("Data/Images/wolfenstein_textures.png"), IntVector2(6,19));
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

	m_currentStatePtr = new LoadState();

}


//----------------------------------------------------------------------------------------------------------------
// Removes the given bullet and packs the array
void TheGame::CheckIfPauseStateChanged() {
	if (!DevConsole::GetInstance()->IsOpen() && g_theInputSystem->WasKeyJustPressed('P') && m_isPaused == false) {
		m_gameClock->SetPaused(true);
	}
	else {
		m_gameClock->SetPaused(false);
	}
}


//----------------------------------------------------------------------------------------------------------------
// Process input relating to game logic EXCEPT pausing and time scaling
void TheGame::ProcessInput() {
	ProcessDebugInput();
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::ProcessDebugInput() {

	if (g_theInputSystem->WasKeyJustPressed('V')) {
		DebugRenderWireSphere(100.f, m_camera->transform.position, 1.f, Rgba(), Rgba());
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
// Updates game logic in this order:
//	1. Get time update
//  2. Check if time scale cheat is active
//  3. Check if paused
//
void TheGame::Update() {

	CheckIfPauseStateChanged();

	if (!DevConsole::GetInstance()->IsOpen()) {
		ProcessInput();
	}

	if (m_nextState != STATE_NONE && m_currentStatePtr->IsReadyToExit()) {
		GoToNextState();
	}

	m_currentStatePtr->Update();	
}


//----------------------------------------------------------------------------------------------------------------
// Calls render for all game objects
void TheGame::Render() {


	m_currentStatePtr->Render();

	g_theRenderer->SetCameraToUI();
}


//----------------------------------------------------------------------------------------------------------------
bool TheGame::IsDevModeActive() const {
	return m_devModeActive;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::LoadTileDefinitions(std::string filePath) {

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(filePath.c_str());
	const tinyxml2::XMLElement* root = doc->RootElement();
	const tinyxml2::XMLElement* tileDef = root->FirstChildElement();

	while (tileDef != nullptr) {
		new TileDefinition(*tileDef, terrain);
		tileDef = tileDef->NextSiblingElement();
	}
	delete doc;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::LoadEntityDefinitions(std::string filePath) {
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(filePath.c_str());
	const tinyxml2::XMLElement* root = doc->RootElement();
	const tinyxml2::XMLElement* entityDef = root->FirstChildElement();

	while (entityDef != nullptr) {
		new EntityDefinition(*entityDef);
		entityDef = entityDef->NextSiblingElement();
	}
	delete doc;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::LoadSpriteDefinitions(std::string filePath) {
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(filePath.c_str());
	const tinyxml2::XMLElement* root = doc->RootElement();
	const tinyxml2::XMLElement* spriteDef = root->FirstChildElement();

	while (spriteDef != nullptr) {
		new Sprite(*spriteDef);
		spriteDef = spriteDef->NextSiblingElement();
	}
	delete doc;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::LoadIsoSpriteDefinitions(std::string filePath) {
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(filePath.c_str());
	const tinyxml2::XMLElement* root = doc->RootElement();
	const tinyxml2::XMLElement* isoSpriteDef = root->FirstChildElement();

	while (isoSpriteDef != nullptr) {
		new IsoSprite(*isoSpriteDef);
		isoSpriteDef = isoSpriteDef->NextSiblingElement();
	}
	delete doc;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::LoadIsoSpriteAnimDefinitions(std::string filePath) {
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(filePath.c_str());
	const tinyxml2::XMLElement* root = doc->RootElement();
	const tinyxml2::XMLElement* isoSpriteAnimDef = root->FirstChildElement();

	while (isoSpriteAnimDef != nullptr) {
		new IsoSpriteAnimDef(*isoSpriteAnimDef);
		isoSpriteAnimDef = isoSpriteAnimDef->NextSiblingElement();
	}
	delete doc;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::LoadCampaignDefinitions(std::string filePath) {
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile(filePath.c_str());
	const tinyxml2::XMLElement* root = doc->RootElement();
	const tinyxml2::XMLElement* campaignDef = root->FirstChildElement();

	while (campaignDef != nullptr) {
		new CampaignDefinition(*campaignDef);
		campaignDef = campaignDef->NextSiblingElement();
	}
	delete doc;
}


//----------------------------------------------------------------------------------------------------------------
Camera* TheGame::GetPlayerCamera() {
	return GetCurrentMap()->GetPlayerCamera();
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::BeginTransitionToState( eGameState next ) {
	m_nextState = next;
	m_currentStatePtr->OnBeginExit();
}


//----------------------------------------------------------------------------------------------------------------
Player* TheGame::GetPlayer() {
	if ( m_currentState == STATE_PLAY ) {
		return (Player*) (GetCurrentMap()->player);
	}
	return nullptr;
}


//----------------------------------------------------------------------------------------------------------------
PlayState* TheGame::GetCurrentPlayState() {
	if ( m_currentState == STATE_PLAY ) {
		return (PlayState*) m_currentStatePtr;
	}
	return nullptr;
}


//----------------------------------------------------------------------------------------------------------------
GameMap* TheGame::GetCurrentMap() {
	return GetCurrentPlayState()->testGameMap;
}


//----------------------------------------------------------------------------------------------------------------
void TheGame::GoToNextState() {

	delete m_currentStatePtr;

	if (m_nextState == STATE_MENU) {
		m_currentStatePtr = new MenuState();
	}
	else if (m_nextState == STATE_PLAY) {
		m_currentStatePtr = new PlayState();
	}
	else if (m_nextState == STATE_SETUP) {
		m_currentStatePtr = new SetupState();
	}
	m_currentState = m_nextState;
	m_nextState = STATE_NONE;

	m_currentStatePtr->OnEnter();
}
