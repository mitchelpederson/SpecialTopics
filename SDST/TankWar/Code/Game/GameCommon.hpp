#pragma once
#include "Game/App.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/InputSystem/InputSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Clock.hpp"

#define UNUSED(x) (void)(x);

constexpr float CLIENT_ASPECT = 1.0f; // We are requesting a 1:1 aspect (square) window area

extern App* g_theApp;
extern TheGame* g_theGame;
extern Renderer* g_theRenderer;
extern TheGame* g_theGame;
extern InputSystem* g_theInputSystem;
extern Clock* g_masterClock;
extern AudioSystem* g_audioSystem;

extern bool g_devModeActive;

const float MAP_MAX_HEIGHT = 18.f;
const float MAP_MIN_HEIGHT = 0.f;
const unsigned int MAP_CHUNK_SIZE = 16.f;
const unsigned int MAP_CHUNKS_X = 12;
const unsigned int MAP_CHUNKS_Y = 12;

const float PLAYER_TURRET_TURN_RATE = 40.f;
const float PLAYER_TANK_TURN_RATE = 90.f;
const float PLAYER_MOVE_FORCE = 50.f;

const int NUM_ENEMY_BASES = 2;
const float BASE_SPAWN_RATE = 2.f;
const float BASE_HEALTH = 10.f;
const int BASE_MAX_SPAWNS = 100;

const float SWARMER_SEPARATION_FACTOR = 10.f;
const float SWARMER_COHESION_FACTOR = 3.f;
const float SWARMER_ALIGNMENT_FACTOR = 8.f;
const float SWARMER_FLOCK_RADIUS = 6.f;
const float SWARMER_MOVE_SPEED = 40.f;
