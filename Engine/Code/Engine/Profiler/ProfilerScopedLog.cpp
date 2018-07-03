#include "Engine/Profiler/ProfilerScopedLog.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/DevConsole/DevConsole.hpp"


ProfilerLogScoped::ProfilerLogScoped( const char* tag ) {
	m_tag = tag;
	m_startHPC = GetPerformanceCount();
}


ProfilerLogScoped::~ProfilerLogScoped( ) {

	uint64_t length = GetPerformanceCount() - m_startHPC;

	DevConsole::Printf( "%s: %f", m_tag, PerformanceCountToSeconds( length ) );

}