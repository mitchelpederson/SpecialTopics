#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/DevConsole/DevConsole.hpp"

#include "Game/GameState/MenuHostState.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheGame.hpp"


//----------------------------------------------------------------------------------------------------------------
MenuHostState::MenuHostState() {

}


//----------------------------------------------------------------------------------------------------------------
void MenuHostState::OnEnter() {
	BeginFadeIn(0.2f);

	if ( g_theGame->netSession->IsDisconnected() ) {
		g_theGame->netSession->Host( "ALPHA", 10084, 32 );

		PlayerInfo* myInfo = new PlayerInfo(g_theGame->netSession->GetMyConnectionIndex());
		myInfo->SetName( g_theGame->netSession->GetMyConnection()->GetID() );
		g_theGame->AddConnectedPlayer(myInfo);

	}

	float w = Window::GetInstance()->GetWidth();
	float h = Window::GetInstance()->GetHeight();
	m_nameInputBox.SetBounds(AABB2(w * 0.5f, h * 0.4f, w * 0.97f, h * 0.425f));
	m_nameInputBox.SetFontSize(h * 0.025f);
	m_nameInputBox.SetFocus(false);
	m_nameInputBox.SetText( "ALPHA" );
}


//----------------------------------------------------------------------------------------------------------------
void MenuHostState::OnBeginExit() {
	BeginFadeOut(0.5f);
}


//----------------------------------------------------------------------------------------------------------------
void MenuHostState::Update() {

	if ( m_nameInputBox.IsInFocus() ) {
		m_nameInputBox.Update();
		if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_ENTER ) ) {
			g_theGame->UpdateMyCallsign( m_nameInputBox.GetText() );
			m_nameInputBox.SetFocus(false);
		}
	}

	else {
		if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_ENTER ) || g_theInputSystem->GetController(0).WasButtonJustPressed( InputSystem::XBOX_START ) ) {
			g_theGame->BeginTransitionToState( STATE_PLAY_HOST );

			NetMessage beginGame(NETMSG_BEGIN_GAME);
			g_theGame->netSession->SendToAllOtherConnections( beginGame );

			for ( int i = 0; i < MAX_CLIENTS; i++ ) {
				PlayerInfo* info = g_theGame->GetPlayerInfo( i );
				if ( info != nullptr ) {
					info->Reset();
				}
			}
		}

		if ( g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_UP) || g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_DOWN)) {
			m_nameInputBox.SetFocus(true);
		}
		m_nameInputBox.Update();
	}

	if ( g_theInputSystem->WasKeyJustPressed( InputSystem::KEYBOARD_ESC ) || g_theInputSystem->GetController(0).WasButtonJustPressed( InputSystem::XBOX_B ) ) {
		g_theGame->BeginTransitionToState( STATE_MENU_MAIN );
		g_theGame->netSession->Disconnect();
	}
	
	GameState::Update();
}


//----------------------------------------------------------------------------------------------------------------
void MenuHostState::Render() {

	float w = Window::GetInstance()->GetWidth();
	float h = Window::GetInstance()->GetHeight();

	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, w, h), Rgba(0, 35, 50, 255));

	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 5.f) * 3, w, h), Vector2(0.5f, 0.f), "HOSTING", h / 8.f, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);

	float fontSize = h / 32.f;
	for ( int i = 0; i <  g_theGame->netSession->GetNumberOfUsers(); i++ ) {
		PlayerInfo* info = g_theGame->GetPlayerInfo(i);
		if ( info != nullptr ) {
			std::string playerInfoStr = "[" + std::to_string(i) + "] " + info->GetName();
			g_theRenderer->DrawTextInBox2D(AABB2(0.f, (h / 2.f) - (fontSize * (i + 1)), w, (h / 2.f) - (fontSize * (i))), Vector2(0.f, 0.f), playerInfoStr, fontSize, Rgba(137, 230, 240, 255), 1.f, g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono"), TEXT_DRAW_OVERRUN);
		}
	}

	m_nameInputBox.Render();

	//g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 50.f), Vector2(0.5f, 0.f), "By Mitchel Pederson", 4.f, Rgba(200, 200, 200, 255), 0.5f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_OVERRUN);
	GameState::Render();
}
