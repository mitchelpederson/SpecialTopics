#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/DevConsole/DevConsole.hpp"
#include "Engine/DevConsole/Command.hpp"

//////////////////////////////////////////////////////////////////////////
// Profiler Console Command Callbacks

void ProfilerPauseCommand( const std::string& command ) {
	Profiler::Pause();
}

void ProfilerUnpauseCommand( const std::string& command ) {
	Profiler::Unpause();
}


//////////////////////////////////////////////////////////////////////////
// ProfilerMeasurement

ProfilerMeasurement::~ProfilerMeasurement() {
	for (unsigned int index = 0; index < children.size(); index++) {
		delete children[index];
		children[index] = nullptr;
	}
}


void ProfilerMeasurement::AddChild( ProfilerMeasurement* child ) {
	children.push_back(child);
}


void ProfilerMeasurement::SetParent( ProfilerMeasurement* p ) {
	parent = p;
}


void ProfilerMeasurement::FinishMeasurement() {
	hpcEnd = GetPerformanceCount();
}


float ProfilerMeasurement::GetLengthSeconds() {
	uint64_t length = hpcEnd - hpcStart;
	return (float) PerformanceCountToSeconds(length);
}



//////////////////////////////////////////////////////////////////////////
// Profiler system

Profiler* Profiler::instance = nullptr;

Profiler::Profiler() {

}


Profiler::~Profiler() {

}


void Profiler::Initialize() {
#ifdef PROFILER_ENABLED

	if (nullptr == instance) {
		instance = new Profiler();
	}

	CommandRegistration::RegisterCommand("pf_pause", ProfilerPauseCommand);
	CommandRegistration::RegisterCommand("pf_unpause", ProfilerUnpauseCommand);

#endif
}


void Profiler::MarkFrame() {
#ifdef PROFILER_ENABLED

	if (!instance->IsInstancePaused()) {
		// Pop previous frame and save it
		if (instance->m_root != nullptr) {
			ASSERT_OR_DIE( instance->m_root == instance->m_current, "Profiler was pushed and popped unevenly" );
			instance->SaveFrame();
			Pop();
		}
	}

	// Check if we're supposed to pause or unpause this frame
	if ( instance->m_isAboutToPause == true ) {
		instance->PauseInstance();
	}
	else if ( instance->m_isAboutToUnpause == true ) {
		instance->UnpauseInstance();
	}

	// If not paused, push the next frame
	if (!instance->IsInstancePaused()) {
		Profiler::Push("frame");
	}

#endif
}


void Profiler::Push( const std::string& id ) {
#ifdef PROFILER_ENABLED

	if (!instance->IsInstancePaused()) {
		ProfilerMeasurement* pm = instance->CreateMeasurement( id );

		if ( nullptr == instance->m_root ) {
			pm->SetParent( nullptr );
			instance->m_root = pm;
			instance->m_current = pm;
		}
		else {
			instance->m_current->AddChild( pm );
			pm->SetParent( instance->m_current );
			instance->m_current = pm;
		}
	}
	
#endif
}


void Profiler::Pop() {
#ifdef PROFILER_ENABLED

	if (!instance->IsInstancePaused()) {

		ASSERT_OR_DIE( instance->m_root != nullptr, "Tried to pop the Profiler when there was no root");
		instance->m_current->FinishMeasurement();
	
		//DevConsole::Printf( "%s took %f", instance->m_current->id.c_str(), instance->m_current->GetLengthSeconds() );
	
		if (instance->m_current == instance->m_root) {
			instance->m_current = nullptr;
			instance->m_root = nullptr;
		}
		else {
			instance->m_current = instance->m_current->parent;
		}
	}

#endif
}


void Profiler::SaveFrame() {
#ifdef PROFILER_ENABLED

	m_root->FinishMeasurement();
	m_history.push_back(m_root);

	if (m_history.size() > PROFILER_MAX_FRAME_HISTORY) {
		ProfilerMeasurement* oldest = m_history.front();
		delete oldest;
		oldest = nullptr;
		m_history.pop_front();
	}

#endif
}


ProfilerMeasurement* Profiler::CreateMeasurement( const std::string& id ) {
	ProfilerMeasurement* pm = new ProfilerMeasurement();
	pm->id = id;
	pm->hpcStart = GetPerformanceCount();
	return pm;
}


void Profiler::Pause() {
	instance->SignalAboutToPause();
}


void Profiler::Unpause() {
	instance->SignalAboutToUnpause();
}


void Profiler::SignalAboutToPause() {
	m_isAboutToPause = true;
}


void Profiler::SignalAboutToUnpause() {
	m_isAboutToUnpause = true;
}


void Profiler::PauseInstance() {
	m_isPaused = true;
	m_isAboutToPause = false;
	m_isAboutToUnpause = false;
}


void Profiler::UnpauseInstance() {
	m_isPaused = false;
	m_isAboutToPause = false;
	m_isAboutToUnpause = false;
}


bool Profiler::IsInstancePaused() {
	return m_isPaused;
}

bool Profiler::IsPaused() {
	return instance->IsInstancePaused();
}


ProfilerMeasurement* Profiler::GetPreviousFrame( unsigned int index /* = 0 */ ) {
	if (index >= instance->m_history.size()) {
		return nullptr;
	}

	else {
		return instance->m_history[instance->m_history.size() - 1 - index];
	}
}