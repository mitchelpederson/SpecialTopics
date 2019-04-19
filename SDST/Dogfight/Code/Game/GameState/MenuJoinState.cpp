#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/DevConsole.hpp"

#include "Game/GameState/MenuJoinState.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheGame.hpp"


//----------------------------------------------------------------------------------------------------------------
MenuJoinState::MenuJoinState() {

}


//----------------------------------------------------------------------------------------------------------------
void MenuJoinState::OnEnter() {
	BeginFadeIn(0.2f);
	float w = Window::GetInstance()->GetWidth();
	float h = Window::GetInstance()->GetHeight();
	m_ipInputBox.SetBounds(AABB2(w * 0.5f, h * 0.5f, w * 0.97f, h * 0.525f));
	m_ipInputBox.SetFontSize(h * 0.025f);
	m_ipInputBox.SetFocus(true);

	m_nameInputBox.SetBounds(AABB2(w * 0.5f, h * 0.4f, w * 0.97f, h * 0.425f));
	m_nameInputBox.SetFontSize(h * 0.025f);
	m_nameInputBox.SetFocus(false);
}


//----------------------------------------------------------------------------------------------------------------
void MenuJoinState::OnBeginExit() {
	BeginFadeOut(0.5f);
}


//----------------------------------------------------------------------------------------------------------------
void MenuJoinState::Update() {

	if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_ESC ) || g_theInputSystem->GetController(0).WasButtonJustPressed( InputSystem::XBOX_B ) ) {
		g_theGame->BeginTransitionToState( STATE_MENU_MAIN );
	}

	if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_ENTER ) && g_theGame->netSession->IsDisconnected() ) {

		NetAddress_T hostAddr( m_ipInputBox.GetText().c_str() );

		NetConnectionInfo_T connInfo;
		connInfo.addr = hostAddr;
		g_theGame->netSession->Join( m_nameInputBox.GetText(), connInfo );
	}

	if ( g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_UP) || g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_DOWN)) {
		if ( m_nameInputBox.IsInFocus() ) {
			m_nameInputBox.SetFocus(false);
			m_ipInputBox.SetFocus(true);
		} else {
			m_nameInputBox.SetFocus(true);
			m_ipInputBox.SetFocus(false);
		}
	}

	if ( g_theGame->netSession->IsReady() && g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_ENTER ) ) {
		if ( m_nameInputBox.IsInFocus() ) {
			g_theGame->UpdateMyCallsign(m_nameInputBox.GetText());
		}
	}

	m_nameInputBox.Update();
	m_ipInputBox.Update();
	GameState::Update();
}


//----------------------------------------------------------------------------------------------------------------
void MenuJoinState::Render() {
	NetSession* session = g_theGame->netSession;
	float w = Window::GetInstance()->GetWidth();
	float h = Window::GetInstance()->GetHeight();

	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, w, h), Rgba(0, 35, 50, 255));

	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));

	if ( !session->IsReady() ) {
		g_theRenderer->DrawText2D( Vector2( w * 0.5f, h * 0.525f ), "Enter the IP and port [IP:PORT]", m_nameInputBox.GetFontSize(), Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono") );
		m_ipInputBox.Render();
	}

	g_theRenderer->DrawText2D( Vector2( w * 0.5f, h * 0.425f ), "Enter your callsign", m_nameInputBox.GetFontSize(), Rgba(), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono") );
	m_nameInputBox.Render();

	switch ( g_theGame->netSession->GetState() ) {
	case SESSION_DISCONNECTED:
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f) * 3, w, h), Vector2(0.5f, 0.f), "Disconnected", h / 8.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
		break;
	case SESSION_CONNECTING:
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f) * 3, w, h), Vector2(0.5f, 0.f), "Connecting...", h / 8.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
		break;
	case SESSION_JOINING:
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f) * 3, w, h), Vector2(0.5f, 0.f), "Joining...", h / 8.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
		break;
	case SESSION_READY:
		g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f) * 3, w, h), Vector2(0.5f, 0.f), "Ready!", h / 8.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
		break;
	default:
		break;
	}

	float fontSize = h / 32;
	for ( int i = 0; i < MAX_CLIENTS; i++ ) {
		PlayerInfo* info = g_theGame->GetPlayerInfo(i);
		if ( info != nullptr ) {
			std::string playerInfoStr = "[" + std::to_string(i) + "] " + info->GetName();
			g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 2.f) - (fontSize * (i + 1)), w, (h / 2.f) - (fontSize * (i))), Vector2(0.f, 0.f), playerInfoStr, fontSize, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
		}
	}
	
	GameState::Render();
}


