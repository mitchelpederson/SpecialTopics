#include "Engine/Net/UDPSocket.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/DevConsole/Command.hpp"


//----------------------------------------------------------------------------------------------------------------
UDPSocket::UDPSocket() {

}


//----------------------------------------------------------------------------------------------------------------
UDPSocket::~UDPSocket() {

}


//----------------------------------------------------------------------------------------------------------------
bool UDPSocket::Bind( NetAddress_T& address, uint16_t portRange ) {

	// Datagram socket over IPv4 and UDP
	SOCKET mySocket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

	GUARANTEE_OR_DIE( mySocket != INVALID_SOCKET, "Couldn't create socket in UDPSocket::Bind()");

	// Make this socket non-blocking
	u_long nonblocking = 1;
	::ioctlsocket( mySocket, FIONBIO, &nonblocking );
	
	sockaddr_storage sockAddress;
	size_t sockAddressLength;
	

	unsigned int bindAttempt = 0;
	int result = 1;

	while ( result != 0 && bindAttempt < portRange ) {
		
		address.ToSockaddr( (sockaddr*) &sockAddress, &sockAddressLength );
		result = ::bind( mySocket, (sockaddr*) &sockAddress, (int) sockAddressLength );
		if (result == 0) {
			m_handle = (SOCKET) mySocket;
			m_address = address;
			return true;
		}

		address.port++;
		bindAttempt++;
	}

	return false;
}


//----------------------------------------------------------------------------------------------------------------
size_t UDPSocket::SendTo( NetAddress_T const& address, void const* data, size_t byteCount ) {

	sockaddr_storage addr;
	size_t addrLen;
	address.ToSockaddr((sockaddr*) &addr, &addrLen);

	SOCKET sock = (SOCKET) m_handle;
	int sent = ::sendto( sock, (char const*) data, (int) byteCount, 0, (sockaddr*) &addr, addrLen );
	if ( sent > 0 ) {
		GUARANTEE_OR_DIE( sent == byteCount, "Something weird in UDPSocket::SendTo()" );
		return sent;
	} else {
		// error check
		return 0;
	}
}


//----------------------------------------------------------------------------------------------------------------
size_t UDPSocket::ReceiveFrom( NetAddress_T& out_address, void* out_buffer, size_t const maxReadSize ) {

	if (IsClosed()) {
		return 0;
	}

	sockaddr_storage addr;
	size_t addrLen;
	SOCKET sock = (SOCKET) m_handle;
	m_address.ToSockaddr((sockaddr*) &addr, &addrLen);

	int recvd = ::recvfrom( sock, (char*) out_buffer, (int) maxReadSize, 0, (sockaddr*) &addr, (int*) &addrLen );
	if (recvd > 0) {
		out_address.FromSockaddr((sockaddr*) &addr);
		return recvd;
	} else {

		int err = ::WSAGetLastError();
		// error check
		return 0;
	}
}

