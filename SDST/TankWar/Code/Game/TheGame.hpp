#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Math/Vector2.hpp"
#include <vector>
#include "Engine/Core/Transform.hpp"


#include "Game/Map/GameMap.hpp"
#include "Game/GameObject.hpp"
#include "Game/States/GameState.hpp"
#include "Game/States/PlayState.hpp"


enum DevModeShader {
	DEV_SHADER_NONE,
	DEV_SHADER_VERTEX_NORMALS,
	DEV_SHADER_SURFACE_NORMALS,
	DEV_SHADER_WORLD_NORMALS,
	DEV_SHADER_TANGENTS,
	DEV_SHADER_BITANGENTS,
	DEV_SHADER_COLOR,
	DEV_SHADER_VERTEX_TANGENTS,
	DEV_SHADER_VERTEX_BITANGENTS,
	NUM_DEV_SHADERS
};

enum eGameState {
	STATE_NONE,
	STATE_LOAD,
	STATE_MENU,
	STATE_SETUP,
	STATE_PLAY
};

//-----------------------------------------------------------------------------------------------
// TheGame class created by Mitchel Pederson
// 
//


class TheGame {

public:
	TheGame();
	~TheGame();

	void Initialize();

	bool IsDevModeActive() const;
	void Update();
	void Render();

	float GetDeltaTime();
	float GetElapsedTime();

	void BeginTransitionToState( eGameState next ); 

	PlayState* GetCurrentPlayState();


private:

	void GoToNextState();
	

	eGameState currentState = STATE_LOAD;
	eGameState nextState = STATE_MENU;

	GameState* m_state = nullptr;

	static TheGame* m_instance;

	static bool m_devModeActive;


	// Debug
	static void DebugStartSpam( const std::string& command );
	static bool m_spam;

};



