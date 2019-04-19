#pragma once
#include "Game/Entity.hpp"
#include "Game/EntityController.hpp"
#include "Game/GameState/MultiplayerState.hpp"

#include "Engine/Renderer/RenderSceneGraph.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Renderer/FirstPersonCamera.hpp"
#include "Engine/Math/Ray.hpp"
#include "Engine/Physics/Contacts.hpp"
#include "Engine/Net/NetSession.hpp"

#include <vector>
#include <map>


enum eMultiplayerClientSubstate {
	CLIENT_STATE_PLAYING,
	CLIENT_STATE_JOINING,
	CLIENT_STATE_VICTORY,
	CLIENT_STATE_LOSS
};

class MultiplayerClientState : public MultiplayerState {

public:
	MultiplayerClientState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;


	Entity* CreateEntityFromNet( int entityID, int entityDefID, EntityController* controller = nullptr, int connectionIndex = -1 );


public:
	eMultiplayerClientSubstate currentSubstate = CLIENT_STATE_PLAYING;


private:
	virtual void SpawnPlayer() override;
	virtual void DrawEndScreen() override;


private:

};
