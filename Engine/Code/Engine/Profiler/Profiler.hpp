#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Profiler {

public:
	Profiler();
	~Profiler();

	void Initialize();
	void MarkFrame();

};