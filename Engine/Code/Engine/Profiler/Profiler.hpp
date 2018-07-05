#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <deque>

struct ProfilerMeasurement {
public:
	std::string		id;
	uint64_t		hpcStart;
	uint64_t		hpcEnd;

	ProfilerMeasurement* parent;
	std::vector<ProfilerMeasurement*> children;

	~ProfilerMeasurement();
	void AddChild( ProfilerMeasurement* child );
	void SetParent( ProfilerMeasurement* parent );
	void FinishMeasurement();
	float GetLengthSeconds();
};


#define PROFILER_SCOPED_PUSH() ProfilerScopedEntry __profiler_scoped_ ## __LINE__ ## (__FUNCTION__)


class Profiler {

public:
	Profiler();
	~Profiler();

	static void Initialize();
	static void MarkFrame();
	static void Push( const std::string& id );
	static void Pop();
	static void Pause();
	static void Unpause();
	
	static ProfilerMeasurement* GetPreviousFrame( unsigned int index = 0 );
	static bool IsPaused();

	static Profiler* instance;
	bool m_isAboutToPause = false;
	bool m_isAboutToUnpause = false;

private:
	void SaveFrame();
	ProfilerMeasurement* CreateMeasurement( const std::string& id );
	void SignalAboutToPause();
	void SignalAboutToUnpause();
	void PauseInstance();
	void UnpauseInstance();
	bool IsInstancePaused();

	ProfilerMeasurement* m_root = nullptr;
	ProfilerMeasurement* m_current = nullptr;

	bool m_isPaused = false;


	std::deque<ProfilerMeasurement*> m_history;
};


class ProfilerScopedEntry {
public:

	ProfilerScopedEntry( const std::string& tag ) {
		Profiler::Push(tag);
	}
	~ProfilerScopedEntry() {
		Profiler::Pop();
	}
};
