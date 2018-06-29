#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Core/Time.hpp"

class LocalTimeData 
{
public:
	LocalTimeData();

public:
	uint64_t m_HPCPerSecond; 
	double m_secondsPerHPC; 
}; 


LocalTimeData::LocalTimeData() {
	::QueryPerformanceFrequency( (LARGE_INTEGER*) &m_HPCPerSecond ); 

	// do the divide now, to not pay the cost later
	m_secondsPerHPC = 1.0 / (double) m_HPCPerSecond;
}

//------------------------------------------------------------------------
// Declaring on the global scope - will 
// cause constructor to be called before 
// our entry function. 
static LocalTimeData gLocalTimeData;



//------------------------------------------------------------------------
// Getting the performance counter
uint64_t GetPerformanceCount() 
{
	uint64_t hpc;
	::QueryPerformanceCounter( (LARGE_INTEGER*)&hpc ); 
	return hpc; 
}

//------------------------------------------------------------------------
// Converting to seconds; 
// relies on gLocalTimeData existing;
double PerformanceCountToSeconds( uint64_t hpc ) 
{
	return (double)hpc * gLocalTimeData.m_secondsPerHPC; 
}


uint64_t SecondsToPerformanceCount( double seconds ) {
	return (uint64_t) (seconds * gLocalTimeData.m_HPCPerSecond);
}


//////////////////////////////////////////////////////////////////////////
// InitializeTime and GetCurrentTimeSeconds copied from professor's code
double InitializeTime( LARGE_INTEGER& out_initialTime ) {

	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency(&countsPerSecond);
	QueryPerformanceCounter(&out_initialTime);
	return 1.0f / (double) countsPerSecond.QuadPart;

}


//-----------------------------------------------------------------------------------------------
// 
//
double GetCurrentTimeSeconds() {

	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = (double) elapsedCountsSinceInitialTime * secondsPerCount;
	return currentSeconds;

}