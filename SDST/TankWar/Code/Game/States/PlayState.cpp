#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/Command.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Renderer/DebugRender.hpp"

#include "Game/States/PlayState.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Tank.hpp"


PlayState::PlayState()
	: m_sceneClock(new Clock(g_masterClock))
	, m_playerRespawnTimer(m_sceneClock)
{
	m_playerRespawnTimer.SetTimer(3.f);
}


void PlayState::Initialize() {
	CommandRegistration::RegisterCommand("debug-sphere", SpawnDebugSphereOverPlayer);

	g_theRenderer->CreateOrGetBitmapFont("Courier");
	terrain = new SpriteSheet(g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png"), IntVector2(8,8));

	m_camera = new OrbitCamera();
	m_camera->SetProjection(Matrix44::MakeProjection(45.f, 16.f / 9.f, 0.1f, 1000.f));	
	m_camera->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_camera->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
	m_cameraLight = new Light();
	m_sun = new Light();
	m_sun->SetAsDirectionalLight( Vector3(50.f, 50.f, 50.f), Vector3(-0.5f, -0.5f, -0.5f), Rgba(), 0.4f, 1.f, 0.f );

	//m_camera->transform.position = Vector3( 0.f, 1.f, -4.f );
	//m_camera->transform.euler = Vector3(-10.f, 0.f, 0.f);

	MeshBuilder builder;
	m_sphereMesh = new Mesh();
	//builder.BuildSphere(m_sphereMesh, Vector3(), 0.5f, 20, 20);
	//m_sphereMesh->SetDrawPrimitive(TRIANGLES);
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
	//m_directional->SetAsDirectionalLight(Vector3(0.f, 10.f, 0.f), Vector3::UP * -1.f, Rgba());
	m_scene->AddLight(m_sun);


	player = new Tank(this, TEAM_PLAYER);
	player->SetPosition(Vector3(50.f, 50.f, 50.f));

	for (int i = 0; i < 10; i++) {
		SpawnBaseInRandomSpot();
	}

	m_bgMusic = g_audioSystem->PlaySound(g_audioSystem->CreateOrGetSound("Data/Audio/tankwar.wav"), true, 0.5f);


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
	if (currentSubstate == STATE_PLAYING) {
		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_LEFT)) {
			m_camera->rotation += 120.f * m_sceneClock->frame.seconds;
		}
		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_RIGHT)) {
			m_camera->rotation -= 120.f * m_sceneClock->frame.seconds;
		}

		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_UP)) {
			m_camera->angle += 30.f * m_sceneClock->frame.seconds;
		}
		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_DOWN)) {
			m_camera->angle -= 30.f * m_sceneClock->frame.seconds;
		}

		if (m_camera->angle > 30.f) {
			m_camera->angle = 30.f;
		}
		if (m_camera->angle < -30.f) {
			m_camera->angle = -30.f;
		}
	}

	else if (currentSubstate == STATE_PLAYER_DIED) {
		if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_SPACE)) {
			delete player;
			player = new Tank(this, TEAM_PLAYER);
			currentSubstate = STATE_PLAYING;
		}
	}
}

void PlayState::ProcessDebugInput() {

	if (g_theInputSystem->WasKeyJustPressed('V')) {
		DebugRenderWireSphere(100.f, m_camera->transform.position, 1.f, Rgba(), Rgba());
	}
}


void PlayState::CheckForPlayerVictory() {
	if (currentSubstate == STATE_PLAYING) {
		if (m_enemyCount == 0) {
			currentSubstate = STATE_PLAYER_WINS;
		}
	}
}

void PlayState::Update() {

	CheckForPlayerVictory();

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) && !IsFading() && !DevConsole::GetInstance()->IsOpen()) {
		g_theGame->BeginTransitionToState(STATE_MENU);
		g_audioSystem->StopSound(m_bgMusic);
	}
	GameState::Update();

	CheckIfPauseStateChanged();

	if (!DevConsole::GetInstance()->IsOpen()) {
		ProcessInput();
	}

	
	Vector3 playerForward = player->transform.GetLocalToWorldMatrix().GetForward();
	player->transform.AlignToNewUp(testGameMap->GetNormalAtPoint(Vector2(player->GetPosition().x,player->GetPosition().z)));
	player->Update();

	m_camera->target = player->transform.position + Vector3(0.f, 1.f, 0.f);
	m_camera->radius = 3.5f;
	m_camera->Update();	
	m_cameraLight->SetAsPointLight(m_camera->transform.position, Rgba(), 1.f, 0.04f);

	for (unsigned int i = 0; i < m_sceneObjects.size(); i++) {
		m_sceneObjects[i]->Update();
	}
	CheckForCombatCollisions();
	ClearDeadGameObjects();
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

	m_forwardRenderPath->Render( m_scene );
	g_theRenderer->SetShader(nullptr);
	g_theRenderer->DisableDepth();
	g_theRenderer->SetCameraToUI();

	RenderUI();

	if (currentSubstate != STATE_PLAYING) {
		g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, 100));
	}

	if (currentSubstate == STATE_PLAYER_DIED) {
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 50.f, 100.f, 100.f), Vector2(0.5f, 0.f), "YOU DIED", 20.f, Rgba(200, 0, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);
		
		if (m_playerRespawnTimer.HasElapsed()) {
			std::string respawnText = "Shoot to respawn";
			g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 50.f), Vector2(0.5f, 1.f), respawnText, 8.f, Rgba(200, 0, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);
		}
		else {
			std::string respawnText = "Respawn in " + std::to_string( 3 - RoundToNearestInt(m_playerRespawnTimer.GetElapsedTime()) ) + "...";
			g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 50.f), Vector2(0.5f, 1.f), respawnText, 8.f, Rgba(200, 0, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);
		}
	
	}

	if (currentSubstate == STATE_PLAYER_WINS) {
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 50.f, 100.f, 100.f), Vector2(0.5f, 0.f), "You win!", 20.f, Rgba(0, 200, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 50.f), Vector2(0.5f, 1.f), "Press Enter to return to the main menu", 6.f, Rgba(0, 200, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_WORD_WRAP);

	}


	GameState::Render();
	DebugRenderSet3DCamera(m_camera);
}


void PlayState::RenderUI() const {

	Renderer* r = g_theRenderer;



	r->DrawAABB(AABB2(2.f, 90.f, 25.f, 100.f), Rgba(0, 0, 0, 255));
	r->DrawAABB(AABB2(3.f, 91.f, 24.f, 99.f), Rgba(160, 0, 0, 255));
	r->DrawAABB(AABB2(3.f, 91.f, RangeMapFloat(player->GetNormalizedHealth(), 0.f, 1.f, 1.f, 24.f), 99.f), Rgba(40, 200, 70, 255));

	r->DrawTextInBox2D(AABB2(2.f, 84.f, 25.f, 88.f), Vector2(0.f, 0.f), "Enemies: " +  std::to_string(m_swarmers.size()), 4.f, Rgba(), 0.4f, r->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);
	r->DrawTextInBox2D(AABB2(2.f, 80.f, 25.f, 84.f), Vector2(0.f, 0.f), "Bases: " +  std::to_string(m_bases.size()), 4.f, Rgba(), 0.4f, r->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);

}


void PlayState::CheckIfPauseStateChanged() {
	if (!DevConsole::GetInstance()->IsOpen() && g_theInputSystem->WasKeyJustPressed('P') && m_isPaused == false) {
		m_sceneClock->SetPaused(true);
	}
	else {
		m_sceneClock->SetPaused(false);
	}
}


void PlayState::SpawnBullet( const Vector3& position, const Vector3& forward, float speed, eTeam team ) {
	Bullet* bullet = new Bullet(this, team);
	bullet->transform.position = position;
	bullet->transform.LookToward(forward, Vector3::UP);
	bullet->SetForwardVelocity(speed);
	m_bullets.push_back(bullet);
	m_sceneObjects.push_back(bullet);
}


void PlayState::SpawnBaseInRandomSpot() {
	Base* base = new Base(this);
	base->transform.position.x = GetRandomFloatInRange(5.f, 200.f);
	base->transform.position.z = GetRandomFloatInRange(5.f, 200.f);

	m_sceneObjects.push_back(base);
	m_bases.push_back(base);
	m_enemyCount++;
}


SwarmEnemy* PlayState::SpawnSwarmEnemyAtSpot( const Vector3& position, Base* parent ) {
	SwarmEnemy* enemy = new SwarmEnemy(this, parent);
	enemy->transform.position = position;
	m_swarmers.push_back(enemy);
	m_sceneObjects.push_back(enemy);
	m_enemyCount++;
	return enemy;
}

void PlayState::SpawnDebugSphereOverPlayer( const std::string& command ) {
	DebugRenderWireSphere(10.f, g_theGame->GetCurrentPlayState()->player->transform.position, 2.f);
}


void PlayState::RemoveBase( Base* base ) {
	m_enemyCount--;
	for ( unsigned int i = 0; i < m_bases.size(); i++ ) {
		if ( m_bases[i] == base ) {
			m_bases[i] = m_bases[m_bases.size() - 1];
			m_bases.pop_back();
			break;
		}
	}

	for ( unsigned int i = 0; i < m_sceneObjects.size(); i++ ) {
		if ( m_sceneObjects[i] == base ) {
			m_sceneObjects[i] = m_sceneObjects[m_sceneObjects.size() - 1];
			m_sceneObjects.pop_back();
			break;
		}
	}

	delete base;
	base = nullptr;
}


void PlayState::RemoveBullet( Bullet* bullet ) {
	for ( unsigned int i = 0; i < m_bullets.size(); i++ ) {
		if ( m_bullets[i] == bullet ) {
			m_bullets[i] = m_bullets[m_bullets.size() - 1];
			m_bullets.pop_back();
			break;
		}
	}

	for ( unsigned int i = 0; i < m_sceneObjects.size(); i++ ) {
		if ( m_sceneObjects[i] == bullet ) {
			m_sceneObjects[i] = m_sceneObjects[m_sceneObjects.size() - 1];
			m_sceneObjects.pop_back();
			break;
		}
	}

	delete bullet;
	bullet = nullptr;
}


void PlayState::RemoveSwarmEnemy( SwarmEnemy* enemy ) {
	m_enemyCount--;
	for ( unsigned int i = 0; i < m_swarmers.size(); i++ ) {
		if ( m_swarmers[i] == enemy ) {
			m_swarmers[i] = m_swarmers[m_swarmers.size() - 1];
			m_swarmers.pop_back();
			break;
		}
	}

	for ( unsigned int i = 0; i < m_sceneObjects.size(); i++ ) {
		if ( m_sceneObjects[i] == enemy ) {
			m_sceneObjects[i] = m_sceneObjects[m_sceneObjects.size() - 1];
			m_sceneObjects.pop_back();
			break;
		}
	}

	delete enemy;
	enemy = nullptr;
}


RaycastHit3 PlayState::Raycast( unsigned int maxContacts, const Ray3& ray ) {
	return testGameMap->Raycast(maxContacts, ray);
}

 
void PlayState::CheckForCombatCollisions() {
	// Check if player bullets hit a swarmer

	for ( unsigned int bulletIndex = 0; bulletIndex < m_bullets.size(); bulletIndex++ ) {
		Bullet* bullet = m_bullets[bulletIndex];
		if (!bullet->IsDeletable()) {
			for ( unsigned int swarmerIndex = 0; swarmerIndex < m_swarmers.size(); swarmerIndex++ ) {
				SwarmEnemy* swarmer = m_swarmers[swarmerIndex];

				if (!swarmer->IsDeletable()) {
					if ((bullet->GetPosition() - swarmer->GetPosition()).GetLength() < (bullet->GetCollisionRadius() + swarmer->GetCollisionRadius())) {
						swarmer->Damage(bullet->GetDamage());
						bullet->Kill();
					}
				}
			}
		}
	}

	// Check if player bullets hit a base
	for ( unsigned int bulletIndex = 0; bulletIndex < m_bullets.size(); bulletIndex++ ) {
		Bullet* bullet = m_bullets[bulletIndex];
		if (!bullet->IsDeletable()) {
			for ( unsigned int baseIndex = 0; baseIndex < m_bases.size(); baseIndex++ ) {
				Base* base = m_bases[baseIndex];

				if (!base->IsDeletable()) {
					if ((bullet->GetPosition() - base->GetPosition()).GetLength() < (bullet->GetCollisionRadius() + base->GetCollisionRadius())) {
						base->Damage(bullet->GetDamage());
						bullet->Kill();
					}
				}
			}
		}
	}


	// Check if swarmer hits a player

	if (!player->IsDeletable()) {
		for ( unsigned int swarmerIndex = 0; swarmerIndex < m_swarmers.size(); swarmerIndex++ ) {
			SwarmEnemy* swarmer = m_swarmers[swarmerIndex];

			if (!swarmer->IsDeletable()) {
				if ((player->GetPosition() - swarmer->GetPosition()).GetLength() < (player->GetCollisionRadius() + swarmer->GetCollisionRadius())) {
					player->Damage(swarmer->GetDamageToPlayer());
					swarmer->Kill();
				}
			}
		}
	}
}


void PlayState::ClearDeadGameObjects() {
	for (int swarmerIndex = (int) m_swarmers.size() - 1; swarmerIndex >= 0; swarmerIndex--) {
		if (m_swarmers[swarmerIndex]->IsDeletable()) {
			RemoveSwarmEnemy(m_swarmers[swarmerIndex]);
		}
	}
	for (int baseIndex = (int) m_bases.size() - 1; baseIndex >= 0; baseIndex--) {
		if (m_bases[baseIndex]->IsDeletable()) {
			RemoveBase(m_bases[baseIndex]);
		}
	}
	for (int bulletIndex = (int) m_bullets.size() - 1; bulletIndex >= 0; bulletIndex--) {
		if (m_bullets[bulletIndex]->IsDeletable()) {
			RemoveBullet(m_bullets[bulletIndex]);
		}
	}
}


void PlayState::SignalPlayerDied() {
	currentSubstate = STATE_PLAYER_DIED;
	m_playerRespawnTimer.Reset();
}


std::vector<SwarmEnemy*> PlayState::GetSwarmersInRadius( const Vector3& point, float radius ) {
	std::vector<SwarmEnemy*> swarmersInRange;

	for (unsigned int swarmerIndex = 0; swarmerIndex < m_swarmers.size(); swarmerIndex++) {
		SwarmEnemy* enemy = m_swarmers[swarmerIndex];
		if ((enemy->GetPosition() - point).GetLength() <= radius) {
			swarmersInRange.push_back(enemy);
		}
	}

	return swarmersInRange;
}