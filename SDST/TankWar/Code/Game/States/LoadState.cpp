#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"

#include "Game/States/LoadState.hpp"
#include "Game/GameCommon.hpp"


LoadState::LoadState() {

}

void LoadState::OnEnter() {
	BeginFadeIn(0.f);
}

void LoadState::OnBeginExit() {
	BeginFadeOut(0.2f);
}


void LoadState::Update() {

	if (m_isSecondFrame) {
		LoadResources();
		m_readyToExit = true;
	}

	if (m_isFirstFrame) {
		m_isFirstFrame = false;
		m_isSecondFrame = true;
	}

	GameState::Update();
}


void LoadState::Render() const {
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(200, 140, 255, 255));
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 100.f), Vector2(1.f, 0.f), "Loading...", 5.f, Rgba(0, 0, 0, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Bisasam"), TEXT_DRAW_OVERRUN);
	GameState::Render();
}


void LoadState::LoadResources() {
	g_theRenderer->CreateOrGetTexture("Data/Images/Terrain_8x8.png");
	Sleep(1000);
}