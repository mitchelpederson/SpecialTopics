#pragma once
#include "Engine/Core/EngineCommon.hpp"


#define PROFILE_LOG_SCOPED() ProfilerLogScoped __profiler_log_ ## __LINE__ ## (__FUNCTION__)


class ProfilerLogScoped {

public:
	ProfilerLogScoped( const char* tag);
	~ProfilerLogScoped();


public:
	uint64_t m_startHPC;
	const char* m_tag;
};