#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Renderer/DebugRender.hpp"

#include "Game/States/PlayState.hpp"
#include "Game/GameCommon.hpp"


PlayState::PlayState() {

}


void PlayState::Initialize() {
	CommandRegistration::RegisterCommand("debug-sphere", SpawnDebugSphereOverPlayer);

	g_theRenderer->CreateOrGetBitmapFont("Courier");
	terrain = new SpriteSheet(g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"), IntVector2(8,8));

	m_camera = new FirstPersonCamera();
	m_camera->SetProjection(Matrix44::MakeProjection(45.f, 16.f / 9.f, 0.1f, 100.f));	
	m_camera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_camera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_cameraLight = new Light();
	m_sun = new Light();
	m_sun->SetAsPointLight(Vector3(50.f, 10000.f, 100000.f), Rgba(), 0.7f, 0.f);

	m_camera->transform.position = Vector3( 0.f, 1.f, -4.f );
	m_camera->transform.euler = Vector3(-10.f, 0.f, 0.f);

	m_sceneClock = new Clock(g_masterClock);

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
	m_camera->SetSkybox(m_skyboxTexture);

	testGameMap = new GameMap();
	m_directional = new Light();
	m_directional->SetAsDirectionalLight(Vector3(0.f, 100.f, 0.f), Vector3(-90.f, 0.f, 0.f), Rgba());
	//m_scene->AddLight(m_directional);
	//m_scene->AddLight(m_cameraLight);
	m_scene->AddLight(m_sun);


	player = new GameObject();
	m_camera->transform.parent = &player->transform;
	Renderable* playerRenderable = new Renderable();
	playerRenderable->SetMesh(m_sphereMesh);
	playerRenderable->SetMaterial(g_theRenderer->GetMaterial("player"));
	m_scene->AddRenderable(playerRenderable);
	player->SetRenderable(playerRenderable);

}

void PlayState::OnEnter() {
	BeginFadeIn(0.5f);
	Initialize();
}

void PlayState::OnBeginExit() {
	BeginFadeOut(0.5f);
}

//-----------------------------------------------------------------------------------------------
// Process input relating to game logic EXCEPT pausing and time scaling
//
void PlayState::ProcessInput() {
	ProcessDebugInput();
	ProcessPlayerInput();
}


void PlayState::ProcessPlayerInput() {
	Vector3 cameraForward = m_camera->GetCameraMatrix().GetForward();
	Vector3 cameraRight = m_camera->GetCameraMatrix().GetRight();
	Vector3 forwardMoveForce = Vector3(50.f, 0.f, 0.f);
	if (g_theInputSystem->IsKeyPressed('W')) {
		//player->transform.position = player->transform.position + cameraForward * m_gameClock->frame.seconds * 10.f;
		player->AddForce(Vector3(DotProduct(forwardMoveForce, cameraForward), 0.f, DotProduct(forwardMoveForce, cameraRight)));
	}
	if (g_theInputSystem->IsKeyPressed('S')) {
		player->AddForce(Vector3(DotProduct(forwardMoveForce * -1.f, cameraForward), 0.f, DotProduct(forwardMoveForce * -1.f, cameraRight)));
	}


	if (g_theInputSystem->IsKeyPressed('I')) {
		player->transform.euler.x += 60.f * m_sceneClock->frame.seconds;
	}
	if (g_theInputSystem->IsKeyPressed('K')) {
		player->transform.euler.x -= 60.f * m_sceneClock->frame.seconds;
	}
	if (g_theInputSystem->IsKeyPressed('A')) {
		player->transform.euler.y -= 60.f * m_sceneClock->frame.seconds;
	}
	if (g_theInputSystem->IsKeyPressed('D')) {
		player->transform.euler.y += 60.f * m_sceneClock->frame.seconds;
	}
}

void PlayState::ProcessDebugInput() {

	if (g_theInputSystem->WasKeyJustPressed('V')) {
		DebugRenderWireSphere(100.f, m_camera->transform.position, 1.f, Rgba(), Rgba());
	}
}



void PlayState::Update() {
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) && !IsFading() && !DevConsole::GetInstance()->IsOpen()) {
		g_theGame->BeginTransitionToState(STATE_MENU);
	}
	GameState::Update();

	CheckIfPauseStateChanged();

	if (!DevConsole::GetInstance()->IsOpen()) {
		ProcessInput();
	}

	player->UpdatePhysics();
	player->GetRenderable()->SetModelMatrix(player->transform.GetLocalToWorldMatrix());
	m_camera->Update(m_sceneClock->frame.seconds);	
	m_cameraLight->SetAsPointLight(m_camera->transform.position, Rgba(), 1.f, 0.04f);

}


void PlayState::Render() const {
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("passthrough"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(200, 140, 255, 255));
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 100.f), Vector2(0.5f, 0.5f), "PLAYING", 30.f, Rgba(0, 0, 0, 255), 0.3f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);
	GameState::Render();

	// Begin actual game render
	g_theRenderer->DisableAllLights();
	// setup
	float halfWidth = Window::GetInstance()->GetWidth() / 2.f;
	float halfHeight = Window::GetInstance()->GetHeight() / 2.f;

	g_theRenderer->SetCamera(m_camera);
	m_camera->SetProjection(Matrix44::MakeProjection(45.f, halfWidth / halfHeight, 0.001f, 100.f));

	g_theRenderer->SetAmbientLight(ambientIntensity, ambientColor);
	g_theRenderer->SetSpecular(specularPower, specularAmount);

	m_forwardRenderPath->RenderSceneForCamera( m_camera, m_scene );

	g_theRenderer->SetCameraToUI();
	DebugRenderSet3DCamera(m_camera);
	GameState::Render();

}


void PlayState::CheckIfPauseStateChanged() {
	if (!DevConsole::GetInstance()->IsOpen() && g_theInputSystem->WasKeyJustPressed('P') && m_isPaused == false) {
		m_sceneClock->SetPaused(true);
	}
	else {
		m_sceneClock->SetPaused(false);
	}
}


void PlayState::SpawnDebugSphereOverPlayer( const std::string& command ) {
	DebugRenderWireSphere(10.f, g_theGame->GetCurrentPlayState()->player->transform.position, 2.f);
}