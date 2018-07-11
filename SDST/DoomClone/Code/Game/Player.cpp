#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Window.hpp"

#include <sstream>
#include <iomanip>

Player::Player() 
	: Entity(100)
{
	m_gunSpriteSheet = new SpriteSheet(g_theRenderer->CreateOrGetTexture("Data/Images/weapons.png"), IntVector2(5,4));
}


void Player::ProcessInput() {

	if (!DevConsole::IsOpen()) {
		Vector2 playerForward = Vector2::MakeDirectionAtDegrees( GetOrientationDegrees() );
		Vector2 playerRight = Vector2::MakeDirectionAtDegrees( GetOrientationDegrees() - 90.f );

		if (g_theInputSystem->IsKeyPressed('W')) {
			SetPosition( GetPosition() + (playerForward *  g_theGame->GetDeltaTime() * m_def->GetMaxMoveSpeed()) );
		}
		if (g_theInputSystem->IsKeyPressed('S')) {
			SetPosition( GetPosition() - (playerForward *  g_theGame->GetDeltaTime() * m_def->GetMaxMoveSpeed()) );
		}
		if (g_theInputSystem->IsKeyPressed('A')) {
			SetPosition( GetPosition() - (playerRight * g_theGame->GetDeltaTime() * m_def->GetMaxMoveSpeed()) );
		}
		if (g_theInputSystem->IsKeyPressed('D')) {
			SetPosition( GetPosition() + (playerRight * g_theGame->GetDeltaTime() * m_def->GetMaxMoveSpeed()) );
		}

		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_LEFT)) {
			SetOrientationDegrees( GetOrientationDegrees() + (m_def->GetMaxTurnRate() * g_theGame->GetDeltaTime()) );
		}
		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_RIGHT)) {
			SetOrientationDegrees( GetOrientationDegrees() - (m_def->GetMaxTurnRate() * g_theGame->GetDeltaTime()) );
		}
		if (g_theInputSystem->IsKeyPressed(InputSystem::KEYBOARD_SPACE)) {
			Shoot();
		}
	}
	
}


void Player::Update() {

	ProcessInput();
	Entity::Update();

}


void Player::Render() const {
	g_theRenderer->SetCameraToUI();

	g_theRenderer->SetShader(g_theRenderer->GetShader("passthroughTex-no-depth"));
	AABB2 texCoords = m_currentSpriteAnim->GetCurrentIsoSprite()->GetSpriteForViewAngle(Vector3::FORWARD, Vector3::FORWARD, Vector3::RIGHT)->GetUVs();
	g_theRenderer->DrawTexturedAABB( AABB2(30.f, 0.f, 70.f, 40.f * Window::GetInstance()->GetAspectRatio()), *m_gunSpriteSheet->GetTexture(), texCoords.mins, texCoords.maxs, Rgba() );

	std::stringstream playerInfo;
	playerInfo << "HEALTH " << std::fixed << std::setprecision(0) << m_health << "\nAMMO   " << m_currentAmmo;

	//g_theRenderer->DrawAABB( AABB2(70.f, 0.f, 100.f, 15.f * Window::GetInstance()->GetAspectRatio()), Rgba(100, 100, 100, 255));
	g_theRenderer->DrawTextInBox2D( AABB2(70.f, 0.f, 100.f, 20.f * Window::GetInstance()->GetAspectRatio()), Vector2( 0.f, 0.f ), playerInfo.str() , 8.f, Rgba(), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_SHRINK_TO_FIT);
}


void Player::Shoot() {
	Entity::Shoot();
}


void Player::Kill() {
	m_isAlive = false;
}