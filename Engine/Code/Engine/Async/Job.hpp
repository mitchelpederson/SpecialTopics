#pragma once
#include <mutex>

class Job {

public:
	virtual ~Job() {}
	virtual void Execute() = 0;
	virtual void OnComplete() = 0;

	int AssignID() {
		s_jobIDLock.lock();
		m_id = s_jobID;
		s_jobID++;
		s_jobIDLock.unlock();

		return m_id;
	}

	int GetID() const {
		return m_id;
	}

private:
	static int s_jobID;
	static std::mutex s_jobIDLock;
	int m_id;
};