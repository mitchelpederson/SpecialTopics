#pragma once
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

	bool IsEmpty() {
		return m_queue.empty();
	}


private:
	std::queue<T> m_queue;
	std::mutex m_lock;
};
