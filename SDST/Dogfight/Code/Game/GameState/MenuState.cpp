#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/DevConsole.hpp"

#include "Game/GameState/MenuState.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheGame.hpp"


MenuState::MenuState() {

}

void MenuState::OnEnter() {
	BeginFadeIn(0.2f);
	g_audioSystem->DeactivateGlobalReverb();
}

void MenuState::OnBeginExit() {
	BeginFadeOut(0.5f);
}


void MenuState::Update() {

	const XboxController& xbc = g_theInputSystem->GetController(0);

	if ( (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) || xbc.WasButtonJustPressed( InputSystem::XBOX_START ) || xbc.WasButtonJustPressed( InputSystem::XBOX_A )) && !IsFading() && !DevConsole::GetInstance()->IsOpen()) {
		if ( m_cursor == 1 ) {
			g_theGame->BeginTransitionToState( STATE_MENU_HOSTING );
		}
		else if ( m_cursor == 0 ) {
			g_theGame->BeginTransitionToState( STATE_MENU_JOINING );
		}
	}


	if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_UP ) || xbc.WasButtonJustPressed( InputSystem::XBOX_DPAD_UP )  ) {
		m_cursor--;
		if ( m_cursor < 0 ) {
			m_cursor = m_numOfOptions - 1;
		}
	}
	if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_DOWN ) || xbc.WasButtonJustPressed( InputSystem::XBOX_DPAD_DOWN ) ) {

		m_cursor++;
		if ( m_cursor >= m_numOfOptions ) {
			m_cursor = 0;
		}
	}

	GameState::Update();
}


void MenuState::Render() {

	float w = Window::GetInstance()->GetWidth();
	float h = Window::GetInstance()->GetHeight();

	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, w, h), Rgba(0, 35, 50, 255));

	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTexturedAABB( AABB2( (w * 0.5f) - 200.f, (h / 5.f) * (m_cursor + 1), (w * 0.5f) + 200.f, (h / 5.f) * (m_cursor + 2)), *g_theRenderer->CreateOrGetTexture( "Data/Images/reticle-default.png"), Vector2(0.f, 0.f), Vector2(1.f, 1.f), Rgba());

	g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f) * 3, w, h), Vector2(0.5f, 0.f), "DOGFIGHT", h / 8.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f) * 2, w, (h / 5.f) * 3), Vector2(0.5f, 0.5f), "Host", h / 16.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f), w, (h / 5.f) * 2), Vector2(0.5f, 0.5f), "Join", h / 16.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);

	//g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 50.f), Vector2(0.5f, 0.f), "By Mitchel Pederson", 4.f, Rgba(200, 200, 200, 255), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_OVERRUN);
	GameState::Render();
}
