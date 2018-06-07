#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Game/States/MenuState.hpp"
#include "Game/GameCommon.hpp"


MenuState::MenuState() {

}

void MenuState::OnEnter() {
	BeginFadeIn(0.2f);
}

void MenuState::OnBeginExit() {
	BeginFadeOut(0.5f);
}


void MenuState::Update() {
	if (g_theInputSystem->WasKeyJustPressed(InputSystem::KEYBOARD_ENTER) && !IsFading()) {
		g_theGame->BeginTransitionToState(STATE_SETUP);
	}
	GameState::Update();
}


void MenuState::Render() const {
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(100, 140, 255, 255));
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 100.f), Vector2(0.5f, 0.5f), "TANK WAR", 20.f, Rgba(0, 0, 0, 255), 0.3f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);
	GameState::Render();
}
