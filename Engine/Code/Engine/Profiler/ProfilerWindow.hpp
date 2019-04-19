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

	AABB2 generalRenderBox = AABB2(0.02f, 0.8f, 0.98f, 0.98f);
	AABB2 entryRenderBox = AABB2(0.02f, 0.02f, 0.98f, 0.78f);
	AABB2 historyRenderBox = AABB2(0.4f, 0.82f, 0.98f, 0.98f);
	float textHeight = 0.02f;
	float maxFrameTimeInHistory = 0.0001f;
	unsigned int lastFrameThatSetMax = 0;
	unsigned int frameCount = 0;

	std::deque<ProfilerReport*> savedFrames;
};