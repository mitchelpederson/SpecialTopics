#pragma once
#include "Engine/Net/NetAddress.hpp"

#include <string>

#pragma comment(lib, "ws2_32.lib")

class Net {
public:
	static bool Startup();
	static void Shutdown();

};

void TestConnection( NetAddress_T const& address, std::string const& message );
void TestHost( int port );