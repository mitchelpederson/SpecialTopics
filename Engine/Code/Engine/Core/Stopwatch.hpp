#pragma once
#include "Engine/Core/Clock.hpp"
#include <stdint.h>

class Stopwatch {

public:
	Stopwatch( Clock* ref );

	void SetClock( Clock* ref );
	void SetTimer( float seconds );
	void Reset();
	bool CheckAndReset();
	bool Decrement();
	unsigned int DecrementAll();

	float GetElapsedTime() const;
	float GetNormalizedElapsedTime() const;
	bool HasElapsed() const;

private:
	Clock* m_reference = nullptr;
	uint64_t m_startHPC;
	uint64_t m_intervalHPC;
};