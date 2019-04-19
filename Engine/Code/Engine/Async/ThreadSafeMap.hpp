#pragma once 

#include <map>
#include <mutex>


template<typename key, typename T>
class ThreadSafeMap {
public:

	T* At( key k ) {
		T* obj = nullptr;
		m_lock.lock();
		std::map<key, T>::iterator it = m_map.find(k);
		if ( it != m_map.end() ) {
			return &it->second;
		}
		m_lock.unlock();
		return obj;
	}

	void Emplace( key k, T obj ) {
		m_lock.lock();
		m_map[k] = obj;
		m_lock.unlock();
	}

	void Erase( key k ) {
		m_lock.lock();
		m_map.erase(k);
		m_lock.unlock();
	}

private:
	std::map<key, T> m_map;
	std::mutex m_lock;
};