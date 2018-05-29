#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Blackboard.hpp"
#include "Engine/InputSystem/InputSystem.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Clock.hpp"

extern Renderer* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern Blackboard* g_theBlackboard;
extern Window* g_window;
extern Clock* g_masterClock;


#define MAX_LIGHTS 8