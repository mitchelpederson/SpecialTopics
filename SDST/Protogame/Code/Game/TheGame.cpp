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
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"


TheGame* TheGame::m_instance = nullptr;
extern bool g_isQuitting;

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

	CheckIfPauseStateChanged();

	if (!DevConsole::GetInstance()->IsOpen()) {
		ProcessInput();
	}

	m_camera->Update();
	
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

	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(80, 50,0, 255));
	DebugRenderSet3DCamera(m_camera);

}


bool TheGame::IsDevModeActive() const {
	return m_devModeActive;
}

