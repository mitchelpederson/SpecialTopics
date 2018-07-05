#pragma once

#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerReportEntry.hpp"

class ProfilerReport {

public:

	void GenerateTreeReportFromFrame( ProfilerMeasurement* root );
	void GenerateFlatReportFromFrame( ProfilerMeasurement* root );
	void Finish();
	double GetTotalFrameTime();

public:
	ProfilerReportEntry* root;
};