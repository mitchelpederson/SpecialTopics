#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Audio/AudioCueDefinition.hpp"

#include "Game/GameState/LoadState.hpp"
#include "Game/GameCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
LoadState::LoadState() {

}


//----------------------------------------------------------------------------------------------------------------
void LoadState::OnEnter() {
	BeginFadeIn(0.f);
}


//----------------------------------------------------------------------------------------------------------------
void LoadState::OnBeginExit() {
	BeginFadeOut(0.2f);
}


//----------------------------------------------------------------------------------------------------------------
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


//----------------------------------------------------------------------------------------------------------------
void LoadState::Render() {
	g_theRenderer->SetCameraToUI();
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui"));
	g_theRenderer->DrawAABB(AABB2(0.f, 0.f, 100.f, 100.f), Rgba(0, 0, 0, 255));
	g_theRenderer->SetShader(g_theRenderer->GetShader("ui-font"));
	g_theRenderer->DrawTextInBox2D(AABB2(0.f, 0.f, 100.f, 100.f), Vector2(1.f, 0.f), "Loading...", 5.f, Rgba(200, 200, 200, 255), 0.4f, g_theRenderer->CreateOrGetBitmapFont("Wolfenstein"), TEXT_DRAW_OVERRUN);
	GameState::Render();
}


//----------------------------------------------------------------------------------------------------------------
void LoadState::LoadResources() {
	LoadAudio();

	g_theRenderer->CreateOrGetBitmapFont("Courier");
	g_theRenderer->CreateOrGetBitmapFont("Wolfenstein");
	g_theRenderer->CreateOrGetBitmapFont("ibm-plex-mono");
	g_theRenderer->CreateOrGetTexture("Data/Fonts/ibm-plex-mono.png")->SetSamplerMode(SAMPLER_LINEAR);

	std::map< int, EntityDefinition* >::iterator entityDefIt = EntityDefinition::s_definitions.begin();
	while ( entityDefIt != EntityDefinition::s_definitions.end() ) {
		g_theRenderer->CreateOrGetMesh( entityDefIt->second->GetMeshPath() );
		entityDefIt++;
	}
}


//----------------------------------------------------------------------------------------------------------------
void LoadState::LoadAudio() {
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile("Data/Definitions/audiocues.xml");
	const tinyxml2::XMLElement& root = *doc->FirstChildElement("audiocues");
	const tinyxml2::XMLElement* audioCue = root.FirstChildElement("audiocue");

	while ( audioCue != nullptr ) {
		new AudioCueDefinition( *audioCue );
		audioCue = audioCue->NextSiblingElement("audiocue");
	}

	delete doc;
	doc = nullptr;

}