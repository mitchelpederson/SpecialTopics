#include "Game/States/GameState.hpp"
#include "Game/GameCommon.hpp"

GameState::GameState() 
	: m_stateClock(g_masterClock)
	, m_fadeStopwatch(m_stateClock) {

}

void GameState::OnEnter() {

}



void GameState::OnBeginExit() {
}

bool GameState::IsReadyToExit() {
	return m_readyToExit;
}


void GameState::Update() {
	if (m_isFading) {
		UpdateFade();
		if (!m_isFadeIn) {
			if (m_fadeStopwatch.HasElapsed()) {
				m_readyToExit = true;
			}
		}
	}
}


void GameState::Render() const {

	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("passthrough"));

	if (m_isFading) {
		RenderFade();
	}
}


void GameState::BeginFadeIn( float duration ) {
	m_isFading = true;
	m_isFadeIn = true;
	m_fadeInLength = duration;
	m_fadeStopwatch.SetTimer(duration);
}

void GameState::BeginFadeOut( float duration ) {
	m_isFading = true;
	m_isFadeIn = false;
	m_fadeOutLength = duration;
	m_fadeStopwatch.SetTimer(duration);
}

bool GameState::IsFading() const {
	return m_isFading;
}

void GameState::UpdateFade() {
	if (m_fadeStopwatch.HasElapsed()) {
		m_isFading = false;
	}
}

void GameState::RenderFade() const {
	float percentFade = m_fadeStopwatch.GetNormalizedElapsedTime();
	if (m_isFadeIn) {
		percentFade = 1.f - percentFade;
	}
	unsigned char quadAlpha = (unsigned char) Interpolate(0, 255, percentFade);
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, quadAlpha));
	
}


float GameState::GetDeltaTime() const {
	return m_stateClock->frame.seconds;
}

float GameState::GetElapsedTime() const {
	return m_stateClock->total.seconds;
}