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
extern JobSystem* g_theJobSystem;
extern bool g_devModeActive;

constexpr float AIR_DENSITY = 0.4f;
constexpr float GRAVITY_ACCEL = 9.8f;
constexpr float MAX_ALTITUDE = 10000.f;

constexpr int MATCH_SCORE_LIMIT = 20000;
constexpr float MATCH_TIME_LIMIT = 300.f;

constexpr float NET_SNAPPING_THRESHOLD = 250.f;