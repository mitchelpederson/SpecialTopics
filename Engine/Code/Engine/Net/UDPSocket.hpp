#pragma once

#include "Engine/Net/Socket.hpp"
#include "Engine/Net/NetAddress.hpp"
#include "Engine/Math/MathUtils.hpp"


class UDPSocket : public Socket {

public:
	UDPSocket();
	~UDPSocket();
	bool Bind( NetAddress_T const& address, uint16_t portRange );
	size_t SendTo( NetAddress_T const& address, void const* data, size_t byteCount );
	size_t ReceiveFrom( NetAddress_T& out_address, void* out_buffer, size_t const maxReadSize );


};


class UDPTest
{
public:

	bool start() 
	{
		// get an address to use; 
		m_socket.Bind(NetAddress_T::GetLocal( 10084 ), 0);
	}

	void stop()
	{
		m_socket.Close(); 
	}

	void send_to( NetAddress_T const &addr, void const *buffer, unsigned int byte_count )
	{
		m_socket.SendTo( addr, buffer, byte_count ); 
	}

	void update()
	{
		char buffer[1500];

		NetAddress_T from_addr; 
		size_t read = m_socket.ReceiveFrom( from_addr, buffer, 1500 - 48 ); 

		if (read > 0U) {
			unsigned int maxBytes = Min( read, 128 ); 
			std::string output = "0x"; 
			char* buffer = new char[1500-48];
			char* iter = buffer;
			strcpy_s(buffer, output.size(), output.c_str()); 

			iter += 2U; // skip the 0x
			for (unsigned int i = 0; i < read; ++i) {
				sprintf_s( iter, 3U, "%02X", buffer[i] ); 
				iter += 2U; 
			}
			*iter = NULL; 
			//output.recalculate_sizes(); 

			DevConsole::Printf( "Received: %s", buffer ); 

			delete buffer;
			buffer = nullptr;
			iter = nullptr;
		}
	}

public:
	// if you have multiple address, you can use multiple sockets
	// but you have to be clear on which one you're sending from; 
	UDPSocket m_socket; 
};
