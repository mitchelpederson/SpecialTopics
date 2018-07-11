#include "Engine/Core/Threads.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/DevConsole/DevConsole.hpp"

#include <windows.h>
#include <fstream>

//----------------------------------------------------------------------------------------------------------------
void ThreadTestWork( void* ) {

	std::ofstream outFile;
	outFile.open( "garbage.txt", std::ios::out );

	for (int i = 0; i < 120000; i++) {
		outFile << GetRandomIntInRange(0, 100000);
	}

	outFile.close();
	DevConsole::Printf("ThreadTestWork finished!");
} 


//----------------------------------------------------------------------------------------------------------------
void TestThreadNonThreaded( const std::string& command ) {
	ThreadTestWork(nullptr);
}


//----------------------------------------------------------------------------------------------------------------
void TestThreadThreaded( const std::string& commmand ) {
	CreateAndDetachThread("test", ThreadTestWork);
}


//----------------------------------------------------------------------------------------------------------------
static DWORD WINAPI ThreadEntryPoint( void* arg ) {
	ThreadStartData* initData = (ThreadStartData*) arg;

	ThreadCB cb = initData->callback;
	void* passArgs = initData->arg;

	delete arg;

	cb( passArgs );
	return 0;
}


//----------------------------------------------------------------------------------------------------------------
ThreadHandle CreateNewThread( const std::string& threadName, ThreadCB callback, void* userData /* = nullptr */ ) {

	ThreadStartData* threadData = new ThreadStartData();
	threadData->name = threadName;
	threadData->callback = callback;
	threadData->arg = userData;

	DWORD id = 0;
	ThreadHandle threadHandle = (ThreadHandle) ::CreateThread( NULL, 0, ThreadEntryPoint, threadData, 0, &id);

	return threadHandle;
}


//----------------------------------------------------------------------------------------------------------------
void JoinThread( ThreadHandle threadHandle ) {
	::WaitForSingleObject( threadHandle, INFINITE );
	::CloseHandle( threadHandle );
}


//----------------------------------------------------------------------------------------------------------------
void DetachThread(  ThreadHandle threadHandle ) {
	::CloseHandle( threadHandle );
}


//----------------------------------------------------------------------------------------------------------------
void CreateAndDetachThread( const std::string& threadName, ThreadCB callback, void* userData /* = nullptr */ ) {
	ThreadHandle handle = CreateNewThread( threadName, callback, userData );
	DetachThread( handle );
}


//----------------------------------------------------------------------------------------------------------------
void SleepThread( unsigned int miliseconds ) {
	::Sleep( (DWORD) miliseconds );
}


//----------------------------------------------------------------------------------------------------------------
void YieldThread() {
	::SwitchToThread();
}
