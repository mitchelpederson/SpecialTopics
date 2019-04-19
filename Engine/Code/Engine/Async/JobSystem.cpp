#include "Engine/Async/JobSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"


bool JobSystem::s_workerThreadsContinue = true;

int Job::s_jobID = 0;
std::mutex Job::s_jobIDLock;


//----------------------------------------------------------------------------------------------------------------
void RunWorkerThreadCB( void* userData ) {
	userData;

	while( JobSystem::s_workerThreadsContinue ) {
		Job* runningJob = g_theJobSystem->AcquireJob();
		if ( runningJob != nullptr ) {
			runningJob->Execute();
			g_theJobSystem->ReturnJob( runningJob );
		} else {
			SleepThread(10);
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
JobSystem::JobSystem() {

}


//----------------------------------------------------------------------------------------------------------------
JobSystem::~JobSystem() {

}


//----------------------------------------------------------------------------------------------------------------
void JobSystem::Startup() {

	for ( int i = 0; i < JOB_SYSTEM_WORKER_THREAD_COUNT; i++ ) {
		m_workers[i] = CreateNewThread("Worker " + std::to_string(i), RunWorkerThreadCB );
	}
}


//----------------------------------------------------------------------------------------------------------------
void JobSystem::Shutdown() {

	while ( !m_pendingJobs.IsEmpty() ) {
		::SleepThread(10);
	}

	s_workerThreadsContinue = false;

	for ( int i = 0; i < JOB_SYSTEM_WORKER_THREAD_COUNT; i++ ) {
		JoinThread( m_workers[i] );
	}
}


//----------------------------------------------------------------------------------------------------------------
int JobSystem::SubmitJob( Job* job ) {
	int id = job->AssignID();
	m_pendingJobs.Push( job );
	return id;
}


//----------------------------------------------------------------------------------------------------------------
Job* JobSystem::AcquireJob() {
	Job* claimedJob = nullptr;
	if ( m_pendingJobs.Pop( &claimedJob ) ) {
		return claimedJob;
	} else {
		return nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
void JobSystem::ReturnJob( Job* job ) {
	m_finishedJobsLock.lock();
	m_finishedJobs[ job->GetID() ] = job;
	m_finishedJobsLock.unlock();
}


//----------------------------------------------------------------------------------------------------------------
Job* JobSystem::ClaimFinishedJob( int jobID ) {
	Job* returnedJob = nullptr;

	m_finishedJobsLock.lock();
	std::map< int, Job* >::iterator finishedJob = m_finishedJobs.find( jobID );

	if ( finishedJob != m_finishedJobs.end()) {
		returnedJob = finishedJob->second;
		returnedJob->OnComplete();
		m_finishedJobs.erase(jobID);
	} 
	
	m_finishedJobsLock.unlock();
	return returnedJob;
	
}