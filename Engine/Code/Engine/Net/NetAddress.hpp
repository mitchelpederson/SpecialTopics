#pragma once
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/Logger.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DevConsole/DevConsole.hpp"

#include <string>


struct NetAddress_T {
public:
	NetAddress_T();
	NetAddress_T( sockaddr const* addr );
	NetAddress_T( char const* string );

	bool ToSockaddr( sockaddr* out, size_t* out_addrlen ) const;
	bool FromSockaddr( sockaddr const* sa );

	std::string to_string() const;


public:
	unsigned int ip4_address = 0;
	uint16_t port = 0;


public:
	static NetAddress_T GetLocal( unsigned int serviceNumber );
};




void GetAddressExample( std::string const& command );
bool GetAddressForHost( sockaddr* out, int* out_addrlen, char const* hostname, char const* service );