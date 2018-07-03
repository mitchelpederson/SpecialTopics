#pragma once
#include "Engine/Core/EngineCommon.hpp"

class ProfilerLogScoped {

public:
	ProfilerLogScoped( const char* tag);
	~ProfilerLogScoped();


public:
	uint64_t m_startHPC;
	const char* m_tag;
};