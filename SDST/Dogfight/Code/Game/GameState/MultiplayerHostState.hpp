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


enum eMultiplayerSubstate {
	HOST_STATE_PLAYING,
	HOST_STATE_JOINING,
	HOST_STATE_VICTORY,
	HOST_STATE_LOSS
};

class MultiplayerHostState : public MultiplayerState {

public:
	MultiplayerHostState();
	virtual void OnEnter() override;
	virtual void OnBeginExit() override;
	virtual void Update() override;
	virtual void Render() override;

	Entity* CreateEntity( int entityDefID, EntityController* controller = nullptr, int connectionIndex = -1 );

public:
	eMultiplayerSubstate currentSubstate = HOST_STATE_PLAYING;


private:
	virtual void SpawnPlayer() override;
	virtual void DrawEndScreen() override;
	void CheckWinConditions();

	
private:


};
