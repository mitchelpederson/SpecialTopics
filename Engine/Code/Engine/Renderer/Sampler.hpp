#pragma once


class Sampler {
public:
	Sampler();
	~Sampler();

	bool Create();
	void Destroy();

	unsigned int GetHandle() const;

private:
	unsigned int m_handle = 0;
};