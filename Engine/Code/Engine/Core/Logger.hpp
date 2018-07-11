#pragma once
#include "Engine/Core/Threads.hpp"
#include <queue>
#include <mutex>

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
};


class Logger {

public:
	static void Startup();
	static void Shutdown();
	static void Printf( const char* text, ... );
	static void PrintTaggedf( const char* tag, const char* text, ... );

	static bool isRunning;
	static ThreadHandle threadHandle;


private:
	static void LogWorker( void* data );
	static void FlushMessages( std::fstream& logFile );
	static void StopThread();

	static ThreadSafeQueue<LogEntry> m_messageQueue;

};