#pragma once
#include "Engine/Core/Threads.hpp"
#include <queue>
#include <mutex>
#include <shared_mutex>


template<typename T>
class ThreadSafeQueue {
public:

	void Push( const T& entry ) {
		m_lock.lock();

		m_queue.push(entry);

		m_lock.unlock();
	}


	bool Pop( T* out_entry ) {
		m_lock.lock();
		bool hasEntry = !m_queue.empty();

		if ( hasEntry ) {
			*out_entry = m_queue.front();
			m_queue.pop();
		}

		m_lock.unlock();
		return hasEntry;
	}


private:
	std::queue<T> m_queue;
	std::mutex m_lock;
};



struct LogEntry {
	std::string tag;
	std::string text;
	float time;
};


typedef void (*logger_hook_cb) (const LogEntry*, void*);


struct LogHook {
	logger_hook_cb callback;
	void* arguments;
};


struct LogTestData {
	const char* fileName;
	unsigned int threadCount;
};


class Logger {

public:
	static void Startup();
	static void Shutdown();

	static void AddHook( logger_hook_cb hookCB, void* arguments );
	static void SetTagFilterMode( bool isBlacklist );
	static void AddTagToFilter( const std::string& command );
	static void RemoveTagFromFilter( const std::string& command );
	static void HideAllLogs( const std::string& command );
	static void EnableAllLogs( const std::string& command );

	static void Printf( const char* text, ... );
	static void PrintTaggedf( const char* tag, const char* text, ... );
	static void Warningf( const char* text, ... );
	static void Errorf( const char* text, ... );

	static void LogTest( const std::string& command );
	static void LogFlushTest( const std::string& command );

	static bool isRunning;
	static ThreadHandle threadHandle;


private:
	static void LogWorker( void* data );
	static void FlushMessages();
	static void StopThread();
	static void WriteToFile( const LogEntry* entry, void* filePtr );
	static void OutputToOutputWindow( const LogEntry* entry, void* arg );

	static void LogTestWorker( void* logTestArgs );

	static std::shared_mutex m_hooksLock;
	static std::vector<LogHook> m_hooks;
	static ThreadSafeQueue<LogEntry> m_messageQueue;
	static std::vector<std::string> m_tagFilter;
	static bool m_isFilterBlacklist;
	static std::fstream* recentLogFile;
	static std::fstream* timestampedLogFile;

};