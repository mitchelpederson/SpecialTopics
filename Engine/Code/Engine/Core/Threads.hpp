#pragma once
#include <string>


//----------------------------------------------------------------------------------------------------------------
// Type defs
struct ThreadHandleStruct;
typedef ThreadHandleStruct* ThreadHandle; 

typedef void (*ThreadCB)( void* userData );

struct ThreadStartData {
	std::string name;
	ThreadCB callback;
	void* arg;
};


//----------------------------------------------------------------------------------------------------------------
// Functions
ThreadHandle	CreateNewThread( const std::string& threadName, ThreadCB callback, void* userData = nullptr );
void			JoinThread( ThreadHandle threadHandle );
void			DetachThread( ThreadHandle threadHandle );
void			CreateAndDetachThread( const std::string& threadName, ThreadCB callback, void* userData = nullptr );

void			SleepThread( unsigned int miliseconds );
void			YieldThread();



//----------------------------------------------------------------------------------------------------------------
// Test functions
void TestThreadNonThreaded( const std::string& command );
void TestThreadThreaded( const std::string& commmand );

