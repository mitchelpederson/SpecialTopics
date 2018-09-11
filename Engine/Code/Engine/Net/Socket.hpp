#pragma once

#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/WindowsCommon.hpp"


class Socket {

public:
	Socket();
	virtual ~Socket();

	void SetBlocking( bool shouldBlock );
	bool Close();
	bool IsClosed() const;

	NetAddress_T const& GetAddress() const;


protected:
	NetAddress_T m_address;
	SOCKET m_handle = INVALID_SOCKET;
	
};


bool IsFatalSocketError( int errorCode );