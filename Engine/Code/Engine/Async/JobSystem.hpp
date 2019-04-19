#pragma once
#include "Engine/Async/Threads.hpp"
#include "Engine/Async/ThreadSafeQueue.hpp"
#include "Engine/Async/ThreadSafeMap.hpp"
#include "Engine/Async/Job.hpp"


#define JOB_SYSTEM_WORKER_THREAD_COUNT 7


class JobSystem {

public:
	JobSystem();
	~JobSystem();

	void Startup();
	void Shutdown();

	int SubmitJob( Job* job );			// Called elsewhere, returns job ID
	Job* AcquireJob();					// Called by worker threads
	void ReturnJob( Job* job );			// Called by worker threads 
	Job* ClaimFinishedJob( int jobID ); // Called by any thread


	static bool s_workerThreadsContinue;


private:

	ThreadHandle m_workers[JOB_SYSTEM_WORKER_THREAD_COUNT];
	ThreadSafeQueue<Job*> m_pendingJobs;
	std::map<int, Job*> m_finishedJobs;
	std::mutex m_finishedJobsLock;

};

void RunWorkerThreadCB( void* userData );