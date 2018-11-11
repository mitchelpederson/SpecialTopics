#include "Engine/Core/Time.hpp"
#include "Engine/Core/Stopwatch.hpp"


Stopwatch::Stopwatch( Clock* ref ) {
	m_reference = ref;
}

void Stopwatch::SetClock( Clock* ref ) {
	m_reference = ref;
}

void Stopwatch::SetTimer( float seconds ) {
	m_intervalHPC = SecondsToPerformanceCount( (double) seconds);
	m_startHPC = m_reference->total.hpc;
}


void Stopwatch::Reset() {
	m_startHPC = m_reference->total.hpc;
}

bool Stopwatch::CheckAndReset() {

	if (HasElapsed()) {
		Reset();
		return true;
	}

	return false;
}

bool Stopwatch::Decrement() {
	if (HasElapsed()) {
		m_startHPC += m_intervalHPC;
		return true;
	}
	return false;
}

unsigned int Stopwatch::DecrementAll() {
	if (HasElapsed()) {
		unsigned int timesElapsed = (unsigned int) (m_reference->total.hpc - m_startHPC) / (unsigned int) m_intervalHPC;
		m_startHPC += m_intervalHPC * timesElapsed;
		return timesElapsed;
	}

	return 0;
}


bool Stopwatch::HasElapsed() const {
	if (m_intervalHPC == 0) {
		return false;
	}
	if (m_reference->total.hpc - m_startHPC >= m_intervalHPC) {
		return true;
	}
	return false;
}

float Stopwatch::GetElapsedTime() const {
	return (float) PerformanceCountToSeconds( m_reference->total.hpc - m_startHPC );
}

float Stopwatch::GetNormalizedElapsedTime() const {
	return (float) PerformanceCountToSeconds(m_reference->total.hpc - m_startHPC) / (float) PerformanceCountToSeconds( m_intervalHPC );
}