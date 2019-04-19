#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"

Clock::Clock( Clock* parent ) : m_parent(parent) {
	if (parent != nullptr) {
		m_parent->AddChild(this);
	}
	Reset();
}

Clock::~Clock() {
	for (int i = 0; i < m_children.size(); i++) {
		delete m_children[i];
		m_children[i] = nullptr;
	}
	m_parent->RemoveChild(this);
}


void Clock::Reset() {
	m_startHPC = GetPerformanceCount();
	m_frameCount = 0;
	m_lastFrameHPC = m_startHPC;

	total.hpc = 0;
	total.seconds = 0.f;
	total.hp_seconds = 0.0;
	frame.hpc = 0;
	frame.seconds = 0.f;
	frame.hp_seconds = 0.0;
}


void Clock::BeginFrame() {

	uint64_t hpc = GetPerformanceCount();
	uint64_t delta = hpc - m_lastFrameHPC;
	m_lastFrameHPC = hpc;
	Advance( delta );
}


void Clock::Advance( uint64_t const delta ) {

	++m_frameCount;

	uint64_t deltaModified;
	if ( m_timeScale == 1.0 ) {
		deltaModified = delta;
	} else {
		deltaModified = (uint64_t) ((double) delta * m_timeScale);
	}

	if (m_paused) {
		deltaModified = 0;
	}

	total.hpc += deltaModified;
	total.hp_seconds = PerformanceCountToSeconds(total.hpc);
	total.seconds = (float) total.hp_seconds;
	frame.hpc = deltaModified;
	frame.hp_seconds = PerformanceCountToSeconds(frame.hpc);
	frame.seconds = (float) frame.hp_seconds;

	for (int i = 0; i < m_children.size(); i++) {
		if ( m_children[i] != nullptr ) {
			m_children[i]->Advance(deltaModified);
		}
	}
}


void Clock::AddChild( Clock *child ) {
	m_children.push_back(child);
}


void Clock::RemoveChild( Clock* child ) {
	for ( int i = 0; i < m_children.size(); i++ ){
		if ( m_children[i] == child ) {
			m_children[i] = m_children[m_children.size() - 1];
			m_children.pop_back();
			break;
		}
	}
}


double Clock::GetCurrentTimeSeconds() const {
	return total.hp_seconds;
}


void Clock::SetPaused( bool paused ) {
	m_paused = paused;
}


void Clock::SetScale( float scale ) {
	m_timeScale = scale;
}


Clock* GetMasterClock() {
	return g_masterClock;
}


double GetCurrentTimeSinceStart() {
	return GetMasterClock()->total.hp_seconds;
}


bool Clock::IsPaused() const {
	return m_paused;
}