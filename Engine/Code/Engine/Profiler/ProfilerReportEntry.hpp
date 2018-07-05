#pragma once
#include "Engine/Profiler/Profiler.hpp"


class ProfilerReportEntry {

public:

	void PopulateTree( ProfilerMeasurement* measurement );
	void PopulateFlat( ProfilerMeasurement* measurement, ProfilerReportEntry* root );
	void AccumulateData( ProfilerMeasurement* measurement );
	ProfilerReportEntry* GetOrCreateChild( const std::string& childID );
	void Finish( ProfilerReportEntry* root );

public:

	std::string id;
	unsigned int callCount;
	double totalTime;
	double selfTime;
	double percentTime;
	double selfPercentTime;


	ProfilerReportEntry* parent;
	std::map<std::string, ProfilerReportEntry*> children;
};