#include "Engine/Net/TCPSocket.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//----------------------------------------------------------------------------------------------------------------
TCPSocket::TCPSocket() {
	
}


//----------------------------------------------------------------------------------------------------------------
TCPSocket::TCPSocket( SOCKET copySocketHandle, NetAddress_T const& addr ) {
	socketHandle = copySocketHandle;
	address = addr;
}


//----------------------------------------------------------------------------------------------------------------
TCPSocket::~TCPSocket() {
	Close();
}


//----------------------------------------------------------------------------------------------------------------
void TCPSocket::Close() {
	if (!IsClosed()) {
		::closesocket( socketHandle );
	}
}


//----------------------------------------------------------------------------------------------------------------
bool TCPSocket::IsClosed() const {
	if ( socketHandle == INVALID_SOCKET ) {
		return true;
	}
	return false;
}


//----------------------------------------------------------------------------------------------------------------
bool TCPSocket::Connect( NetAddress_T const& addr ) {
	// Create the saddr from our NetAddress object
	sockaddr_storage saddr;
	size_t addrlen;
	addr.ToSockaddr( (sockaddr*) &saddr, &addrlen );

	// Open the socket
	socketHandle = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( socketHandle == INVALID_SOCKET ) {
		::closesocket( socketHandle );
		ERROR_RECOVERABLE( "Could not create socket in TCPSocket::Connect()" );
		return false;
	}

	// Attempt to connect
	int connectResult = ::connect( socketHandle, (sockaddr*) &saddr, (int) addrlen );
	if ( connectResult == SOCKET_ERROR ) {

		int errorCode = ::WSAGetLastError();
		::closesocket( socketHandle );
		ERROR_RECOVERABLE( "Could not connect in TCPSocket::Connect()" );
		return false;
	}

	return true;
}


//----------------------------------------------------------------------------------------------------------------
size_t TCPSocket::Send( void const* data, size_t byteSize ) {
	return (size_t) ::send( socketHandle, (char const*) data, (int) byteSize, 0 );
}


//----------------------------------------------------------------------------------------------------------------
size_t TCPSocket::Receive( void* buffer, size_t maxByteSize ) {
	return (size_t) ::recv( socketHandle, (char*) buffer, (int) maxByteSize, 0 );
}


//----------------------------------------------------------------------------------------------------------------
bool TCPSocket::Listen( uint16_t port, unsigned int maxQueueSize ) {

	address = NetAddress_T::GetLocal(port);

	// Open the socket
	socketHandle = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( socketHandle == INVALID_SOCKET ) {
		::closesocket( socketHandle );
		ERROR_RECOVERABLE( "Could not create socket in TCPSocket::Listen()" );
		return false;
	}

	// Make this socket non-blocking
	u_long nonblocking = 1;
	::ioctlsocket( socketHandle, FIONBIO, &nonblocking );

	// Bind my address to a socket
	sockaddr_storage saddr;
	size_t addrlen;
	address.ToSockaddr( (sockaddr*) &saddr, &addrlen );

	int bindResult = ::bind( socketHandle, (sockaddr*) &saddr, addrlen );
	if (bindResult == SOCKET_ERROR) {
		::closesocket( socketHandle );
		ERROR_RECOVERABLE( "Could not bind socket to host on in TestHost()" );
		return false;
	}

	// Try to listen on it
	int listenResult = ::listen( socketHandle, maxQueueSize );
	if (listenResult == SOCKET_ERROR) {
		::closesocket( socketHandle );
		ERROR_RECOVERABLE( "Could not listen on the socket" );
		return false;
	}

	DevConsole::Printf("Listening on port %u", address.port);

	return true;
}


//----------------------------------------------------------------------------------------------------------------
TCPSocket* TCPSocket::Accept() {
	sockaddr_storage saddr;
	int addrlen;	
	SOCKET otherSocket = ::accept( socketHandle, (sockaddr*) &saddr, &addrlen );

	if ( otherSocket != INVALID_SOCKET ) {
		NetAddress_T netAddress;
		netAddress.FromSockaddr( (sockaddr*) &saddr );
		return new TCPSocket(otherSocket, netAddress);
	}
	
	else {
		return nullptr;
	}

}


//----------------------------------------------------------------------------------------------------------------
bool TCPSocket::HasFatalError() {
	int errorCode = ::WSAGetLastError();

	if (errorCode == WSAEWOULDBLOCK || errorCode == WSAEMSGSIZE || errorCode == WSAECONNRESET ) {
		return false;
	}
	else {
		return true;
	}
}