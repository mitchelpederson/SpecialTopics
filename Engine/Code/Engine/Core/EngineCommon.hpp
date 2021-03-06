#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Blackboard.hpp"
#include "Engine/InputSystem/InputSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Async/JobSystem.hpp"

extern Renderer* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern Blackboard* g_theBlackboard;
extern Window* g_window;
extern Clock* g_masterClock;
extern AudioSystem* g_audioSystem;
extern JobSystem* g_theJobSystem;

#define MAX_LIGHTS 8
//#define PROFILER_ENABLED
#define PROFILER_MAX_FRAME_HISTORY 128
#define GAME_PORT 10084