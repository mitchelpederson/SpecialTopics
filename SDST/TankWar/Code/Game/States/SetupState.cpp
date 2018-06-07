#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Game/States/SetupState.hpp"
#include "Game/GameCommon.hpp"


SetupState::SetupState() {

}

void SetupState::OnEnter() {
	BeginFadeIn(0.2f);
}

void SetupState::OnBeginExit() {
	BeginFadeOut(1.f);
}


void SetupState::Update() {
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) && !IsFading()) {
		g_theGame->BeginTransitionToState(STATE_PLAY);
	}
	GameState::Update();
}


void SetupState::Render() const {
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(100, 140, 255, 255));
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 100.f), Vector2(0.5f, 0.5f), "Press Enter to Continue", 10.f, Rgba(0, 0, 0, 255), 0.3f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);
	GameState::Render();
}
