#pragma once
#include <stdint.h>

// Gets raw performance counter
uint64_t GetPerformanceCount(); 

// converts a performance count the seconds it represents
double PerformanceCountToSeconds( uint64_t seconds ); 

// Legacy function to support older games
double GetCurrentTimeSeconds();