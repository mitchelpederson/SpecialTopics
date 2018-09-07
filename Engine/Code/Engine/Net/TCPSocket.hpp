#pragma once

#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/WindowsCommon.hpp"
#include "Engine/Core/BytePacker.hpp"


class TCPSocket {

public:
	TCPSocket();
	TCPSocket( SOCKET copySocketHandle, NetAddress_T const& addr );
	~TCPSocket();

	bool Listen( uint16_t port, unsigned int maxQueueSize );
	TCPSocket* Accept();
	
	bool Connect( NetAddress_T const& addr );
	
	void Close();

	size_t Send( void const* data, size_t byteSize );
	size_t Receive( void* buffer, size_t maxByteSize );

	bool IsClosed() const;

	bool HasFatalError();

public:
	SOCKET socketHandle;
	NetAddress_T address;
	BytePacker buffer;
};