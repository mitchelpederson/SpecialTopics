#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Game/Asteroid.hpp"
#include "Engine/Audio/AudioSystem.hpp"



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
	g_isQuitting = true;
}

//-----------------------------------------------------------------------------------------------
// Spawns the initial asteroids
//
void TheGame::Initialize() {

	CommandRegistration::RegisterCommand("quit", QuitGame);

	g_theRenderer->CreateOrGetBitmapFont("Courier");

	m_camera = new FirstPersonCamera();
	m_camera->SetProjection(Matrix44::MakeProjection(45.f, 16.f / 9.f, 0.1f, 100.f));	
	m_camera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_camera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_camera->cameraEffects.push_back(g_theRenderer->GetMaterial("testPostProcessEffect"));
	m_camera->cameraEffects.push_back(g_theRenderer->GetMaterial("dashDistort"));
	m_cameraLight = new Light();
	//m_cameraLight->SetAsPointLight(m_camera->position, Rgba(), 1.f, 0.f);

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

	m_playerShip = new Ship(m_camera);

	for (int i = 0; i < 20; i++) {
		/*Light* left = new Light();
		Light* right = new Light();
		left->SetAsPointLight(  Vector3(-7.f, 4.f, i * 6.f), Rgba(i * 10, 125, 125, 255) );
		right->SetAsPointLight( Vector3( 7.f, 4.f, i * 6.f), Rgba(125, i * 10, 125, 255) );
		m_scene->AddLight( left );
		m_scene->AddLight( right );*/
		SpawnAsteroidAtRandom();
	}

	m_directional = new Light();
	m_directional->SetAsDirectionalLight(Vector3(-10000.f, 0.f, 0.f), Vector3(-0.1f, 0.f, -1.f), Rgba(30, 170, 255, 255), 0.3f, 0.f);
	m_scene->AddLight( m_directional );

	m_skyboxTexture = g_theRenderer->CreateCubeMap("Data/Images/galaxy2.png");
	m_skyboxShader = g_theRenderer->GetShader("skybox");
	m_camera->SetSkybox(m_skyboxTexture);

	music = g_audioSystem->CreateOrGetSound("Data/Audio/asteroids.wav");
	musicPlaybackID = g_audioSystem->PlaySound(music, true);
	g_audioSystem->AddFFTToChannel(musicPlaybackID);

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
	
	if (g_theInputSystem->IsKeyPressed('1')) {
		ambientIntensity -= m_gameClock->frame.seconds;
		ambientIntensity = ClampFloat(ambientIntensity, 0.f, 1.f);
	}
	if (g_theInputSystem->IsKeyPressed('2')) {
		ambientIntensity += m_gameClock->frame.seconds;
		ambientIntensity = ClampFloat(ambientIntensity, 0.f, 1.f);
	}

	if (g_theInputSystem->IsKeyPressed('3')) {
		specularAmount -= m_gameClock->frame.seconds;
		specularAmount = ClampFloat(specularAmount, 0.f, 1.f);
	}
	if (g_theInputSystem->IsKeyPressed('4')) {
		specularAmount += m_gameClock->frame.seconds;
		specularAmount = ClampFloat(specularAmount, 0.f, 1.f);
	}
	if (g_theInputSystem->IsKeyPressed('5')) {
		specularPower -= 10.f * m_gameClock->frame.seconds;
		specularPower = ClampFloat(specularPower, 1.f, 50.f);
	}
	if (g_theInputSystem->IsKeyPressed('6')) {
		specularPower += 10.f * m_gameClock->frame.seconds;
		specularPower = ClampFloat(specularPower, 1.f, 100.f);
	}
}


void TheGame::ProcessPlayerInput() {
	Vector3 playerForward = m_playerShip->transform.GetLocalToWorldMatrix().GetForward();
	Vector3 playerUp = m_playerShip->transform.GetLocalToWorldMatrix().GetUp();
	Vector3 playerRight = m_playerShip->transform.GetLocalToWorldMatrix().GetRight();
	float dt = m_gameClock->frame.seconds;

	// Keyboard Input
	if (g_theInputSystem->IsKeyPressed('W')) {		
		m_playerShip->AddTorque( Vector3(60.f, 0.f, 0.f) );
	}
	if (g_theInputSystem->IsKeyPressed('S')) {
		m_playerShip->AddTorque( Vector3(-60.f, 0.f, 0.f) );
	}
	if (g_theInputSystem->IsKeyPressed('J')) {
		m_playerShip->AddForce( playerRight * -10.f );
	}
	if (g_theInputSystem->IsKeyPressed('L')) {
		m_playerShip->AddForce( playerRight * 10.f );
	}
	if (g_theInputSystem->IsKeyPressed('I')) {
		m_playerShip->AddForce( playerForward * 10.f );
	}
	if (g_theInputSystem->IsKeyPressed('K')) {
		m_playerShip->AddForce( playerForward * -10.f );
	}
	if (g_theInputSystem->IsKeyPressed('A')) {
		m_playerShip->AddTorque( Vector3( 0.f, -60.f, 0.f) );
	}
	if (g_theInputSystem->IsKeyPressed('D')) {
		m_playerShip->AddTorque( Vector3( 0.f, 60.f, 0.f) );
	}
	if (g_theInputSystem->IsKeyPressed('Q')) {
		m_playerShip->AddTorque( Vector3( 0.f, 0.f, -60.f) );
	}
	if (g_theInputSystem->IsKeyPressed('E')) {
		m_playerShip->AddTorque( Vector3( 0.f, 0.f, 60.f ) );
	}


	// Controller Input
	const XboxController& controller = g_theInputSystem->GetController(0);
	if (controller.GetRightTrigger() > 0.f) {
		m_playerShip->AddForce( playerForward * controller.GetRightTrigger() * 30.f );
	}
	if (controller.GetLeftTrigger() > 0.f) {
		m_playerShip->AddForce( playerForward * controller.GetLeftTrigger() * -30.f );
	}
	if (controller.GetRightStickMagnitude() > 0.f) { 
		m_playerShip->AddTorque( Vector3( 0.f, 0.f, -100.f * CosDegrees(controller.GetRightStickAngle())) );
	}
	if (controller.GetLeftStickMagnitude() > 0.f) { 
		m_playerShip->AddTorque( Vector3( 0.f, 40.f * CosDegrees(controller.GetLeftStickAngle()), 0.f ) );
		m_playerShip->AddTorque( Vector3( -100.f * SinDegrees(controller.GetLeftStickAngle()), 0.f, 0.f ) );
	}
	if (controller.WasButtonJustPressed(InputSystem::XBOX_RB)) {
		m_playerShip->StrafeDashRight();
	}
	if (controller.WasButtonJustPressed(InputSystem::XBOX_LB)) {
		m_playerShip->StrafeDashLeft();
	}
	if (controller.IsButtonPressed(InputSystem::XBOX_A)) {
		m_playerShip->Shoot();
	}
}


void TheGame::ProcessDebugInput() {

	if (g_theInputSystem->WasKeyJustPressed('V')) {
		DebugRenderWireSphere(100.f, m_camera->transform.position, 1.f, Rgba(), Rgba());
	}

	if (g_theInputSystem->IsKeyPressed('X')) {
		//m_shipTransform.Rotate(Vector3(0.f, 120.f * m_gameClock->frame.seconds, 0.f));
	}
	if (g_theInputSystem->IsKeyPressed('Z')) {
		//m_shipTransform.Rotate(Vector3(0.f, -120.f * m_gameClock->frame.seconds, 0.f));
	}


	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F6)) {
		m_devModeShader = DEV_SHADER_VERTEX_NORMALS;
	}
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F7)) {
		m_devModeShader = DEV_SHADER_SURFACE_NORMALS;
	}
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F8)) {
		m_devModeShader = DEV_SHADER_WORLD_NORMALS;
	}
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F9)) {
		m_devModeShader = DEV_SHADER_COLOR;
	}
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F1)) {
		m_devModeShader = DEV_SHADER_VERTEX_TANGENTS;
	}
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_F2)) {
		m_devModeShader = DEV_SHADER_VERTEX_BITANGENTS;
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

	Rgba color(255, 0, 0, 255);

	color.SetFromText("255, 0, 255, 255");

	if (!DevConsole::GetInstance()->IsOpen()) {
		ProcessInput();
	}

	m_playerShip->Update();

	for (int i = 0; i < m_asteroids.size(); i++) {
		m_asteroids[i]->Update();
	}

	for (int i = 0; i < m_bullets.size(); i++) {
		m_bullets[i]->Update();
	}

	CheckBulletAsteroidCollisions();

	for (int i = m_sparkEmitters.size() - 1; i > 0; i--) {
		m_sparkEmitters[i]->Update();
		if (m_sparkEmitters[i]->IsSafeToDestroy()) {
			m_scene->RemoveParticleEmitter(m_sparkEmitters[i]);
			delete m_sparkEmitters[i];
			m_sparkEmitters[i] = m_sparkEmitters[m_sparkEmitters.size() - 1];
			m_sparkEmitters.pop_back();
		}
	}

	float lightIntensity = g_audioSystem->GetBassVolume() * 10.f;
	lightIntensity = ClampFloat(lightIntensity, 0.05f, 0.4f);

	m_directional->SetAsDirectionalLight(Vector3(-10000.f, 0.f, 0.f), Vector3(-0.1f, 0.f, -1.f), Rgba(30, 170, 255, 255), lightIntensity, 0.f);
	m_camera->Update(m_gameClock->frame.seconds);
	
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
	DebugRenderSet3DCamera(m_camera);
}


bool TheGame::IsDevModeActive() const {
	return m_devModeActive;
}



void TheGame::SpawnAsteroidAtRandom() {
	
	float spawnX = GetRandomFloatInRange(50.f, 600.f);
	float negativeX = GetRandomFloatZeroToOne();
	if (negativeX >= 0.5f) {
		spawnX *= -1.f;
	}
	float spawnZ = GetRandomFloatInRange(50.f, 600.f);
	float negativeZ = GetRandomFloatZeroToOne();
	if (negativeZ >= 0.5f) {
		spawnZ *= -1.f;
	}


	Vector3 spawnPoint = Vector3( spawnX
								, GetRandomFloatInRange(-100.f, 100.f)
								, spawnZ );
	Vector3 spawnAngle = Vector3( GetRandomFloatInRange(-180.f, 180.f)
								, GetRandomFloatInRange(-180.f, 180.f)
								, GetRandomFloatInRange(-180.f, 180.f));
	Vector3 spawnForce = Vector3( GetRandomFloatInRange(-20.f, 20.f)
								, GetRandomFloatInRange(-5.f, 5.f)
								, GetRandomFloatInRange(-20.f, 20.f));
	Vector3 spawnSpin  = Vector3( GetRandomFloatInRange(-20.f, 20.f)
							   , GetRandomFloatInRange(-20.f, 20.f)
							   , GetRandomFloatInRange(-20.f, 20.f));
	
	Asteroid* asteroid = new Asteroid();
	asteroid->AddForce(spawnForce);
	asteroid->AddTorque(spawnSpin);
	asteroid->transform.position = spawnPoint;
	asteroid->transform.euler = spawnAngle;
	m_scene->AddRenderable(asteroid->GetRenderable());
	m_asteroids.push_back(asteroid);
	
}


void TheGame::AddBullet(Bullet* bullet) {
	m_bullets.push_back(bullet);
}


void TheGame::AddAsteroid(Asteroid* asteroid) {
	m_asteroids.push_back(asteroid);
}


void TheGame::RemoveBullet(Bullet* bullet) {
	std::vector<Bullet*>::iterator searchResult = std::find(m_bullets.begin(), m_bullets.end(), bullet);
	if (searchResult != m_bullets.end()) {
		unsigned int index = searchResult - m_bullets.begin();
		m_bullets[index] = m_bullets[ m_bullets.size() - 1 ];
		m_bullets.pop_back();
	}
}


void TheGame::RemoveAsteroid(Asteroid* asteroid) {
	std::vector<Asteroid*>::iterator searchResult = std::find(m_asteroids.begin(), m_asteroids.end(), asteroid);
	if (searchResult != m_asteroids.end()) {
		unsigned int index = searchResult - m_asteroids.begin();
		m_asteroids[index] = m_asteroids[ m_asteroids.size() - 1 ];
		m_asteroids.pop_back();
	}
}


void TheGame::CheckBulletAsteroidCollisions() {

	for (int i = 0; i < m_asteroids.size(); i++) {
		Asteroid& asteroid = *m_asteroids[i];
		
		for (int j = 0; j < m_bullets.size(); j++) {
			Bullet& bullet = *m_bullets[j];

			Vector3 displacementBetween = asteroid.GetPosition() - bullet.GetPosition();
			float distanceBetween = displacementBetween.GetLength();
			if (asteroid.GetAsteroidScale() + 0.4f > distanceBetween) {
				asteroid.Hit();
				bullet.Kill();
				
				SpawnSparkEmitter(bullet.GetPosition());
			}
		}
	}
}


void TheGame::SpawnSparkEmitter( const Vector3& location ) {
	ParticleEmitter* sparks = new ParticleEmitter(m_gameClock);
	sparks->transform.position = location;
	sparks->burstSize = IntRange( 60, 100 );
	sparks->SetColorOverTime(Rgba(255, 0, 200, 255), Rgba(255, 0, 200, 0));
	sparks->SetSpawnSpeed(FloatRange(30.f, 40.f));
	sparks->particleLifespan = FloatRange(1.f, 2.f);
	sparks->spawnConeAngle = 360.f;
	sparks->spawnRate = 0.f;

	sparks->renderable->SetMaterial(g_theRenderer->GetMaterial("particle"));
	sparks->SpawnBurst();
	m_scene->AddParticleEmitter(sparks);
	m_sparkEmitters.push_back(sparks);
}


Vector3 TheGame::GetPlayerLocation() {
	return m_playerShip->GetPosition();
}