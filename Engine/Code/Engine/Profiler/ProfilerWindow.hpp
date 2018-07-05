#pragma once
#include "Engine/Profiler/ProfilerReport.hpp"

#include <deque>



class ProfilerWindow {

	enum eViewMode {
		PROFILER_VIEW_TREE,
		PROFILER_VIEW_FLAT
	}; 
public:

	void Update();
	void Render() const;

	static void Initialize();
	static ProfilerWindow* GetInstance();
	static void Open();
	static void Close();

private:
	void RenderBackground() const;
	void RenderGeneralFrameInfo( ProfilerReportEntry* root ) const;
	void RenderHistoryGraph() const;
	void RenderReportEntries( ProfilerReportEntry* root ) const;
	unsigned int RenderEntry( ProfilerReportEntry* node, unsigned int index, unsigned int indent ) const;
	static ProfilerWindow* instance;
	static bool isOpen;
	static eViewMode currentProfilerViewMode;

	AABB2 generalRenderBox = AABB2(2.f, 80.f, 98.f, 98.f);
	AABB2 entryRenderBox = AABB2(2.f, 2.f, 98.f, 78.f);
	AABB2 historyRenderBox = AABB2(40.f, 82.f, 98.f, 98.f);
	float textHeight = 2.f;
	float maxFrameTimeInHistory = 0.0001f;
	unsigned int lastFrameThatSetMax = 0;
	unsigned int frameCount = 0;

	std::deque<ProfilerReport*> savedFrames;
};