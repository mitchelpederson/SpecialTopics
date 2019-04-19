#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/DevConsole.hpp"

#include "Game/GameState/SetupState.hpp"
#include "Game/GameCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
SetupState::SetupState() {

}


//----------------------------------------------------------------------------------------------------------------
void SetupState::OnEnter() {
	BeginFadeIn(0.2f);
}


//----------------------------------------------------------------------------------------------------------------
void SetupState::OnBeginExit() {
	BeginFadeOut(1.f);
}


//----------------------------------------------------------------------------------------------------------------
void SetupState::UpdateCursorPosition() {

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_UP) || g_theInputSystem->WasKeyJustPressed('W')) {
		if (m_campaignSelectCursor == 0) {
			m_campaignSelectCursor = m_numCampaigns - 1;
		}
		else {
			m_campaignSelectCursor--;
		}
	}

	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_DOWN) || g_theInputSystem->WasKeyJustPressed('S')) {
		if (m_campaignSelectCursor == m_numCampaigns - 1) {
			m_campaignSelectCursor = 0;
		}
		else {
			m_campaignSelectCursor++;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void SetupState::Update() {
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) && !IsFading() && !DevConsole::GetInstance()->IsOpen()) {
	}

	UpdateCursorPosition();

	GameState::Update();
}


//----------------------------------------------------------------------------------------------------------------
void SetupState::Render() {
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, 255));

	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 100.f), Vector2(0.2f, 1.f), "Choose a campaign:", 10.f, Rgba(200, 200, 200, 255), 0.3f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);

	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB( AABB2(50.f, 60.f - (10.f * (float) m_campaignSelectCursor), 85.f, 70.f - (10.f * (float) m_campaignSelectCursor)), Rgba(160, 0, 0, 80) );

	GameState::Render();
}
