#include "Engine/Net/NetAddress.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//----------------------------------------------------------------------------------------------------------------
NetAddress_T::NetAddress_T() 
	: ip4_address( 0 )
	, port( 0 ) 
{

}


//----------------------------------------------------------------------------------------------------------------
NetAddress_T::NetAddress_T( sockaddr const* addr ) {
	sockaddr_in* addr_in = (sockaddr_in*) addr;
	ip4_address = addr_in->sin_addr.S_un.S_addr;
	port = ::ntohs(addr_in->sin_port);
}


//----------------------------------------------------------------------------------------------------------------
NetAddress_T::NetAddress_T( char const* string ) {
	std::vector<std::string> pieces = SplitString(string, ':'); // Attempt to split the string

	sockaddr_in saddr;
	int addrlen;


	// If there's only one string returned, it should just be a number
	// could have more error checking here 
	if (pieces.size() == 1) {

		GetAddressForHost( (sockaddr*) &saddr, &addrlen, pieces[0].c_str(), "80" );
		ip4_address = saddr.sin_addr.S_un.S_addr;
		port = 80;

	}
	// If there's two, it's an ip and a port
	else if (pieces.size() == 2) {
		GetAddressForHost( (sockaddr*) &saddr, &addrlen, pieces[0].c_str(), pieces[1].c_str() );
		ip4_address = saddr.sin_addr.S_un.S_addr;
		port = ::ntohs(saddr.sin_port);
	}
	// If neither, something went really wrong
	else {
		ERROR_AND_DIE("NetAddress_T( char const* string ) could not split the string correctly (input:  %s)", string);
	}
}


//----------------------------------------------------------------------------------------------------------------
bool NetAddress_T::ToSockaddr( sockaddr* out, size_t* out_addrlen ) const {

	sockaddr_in* addr = (sockaddr_in*) out;
	addr->sin_addr.S_un.S_addr = ip4_address;
	addr->sin_port = ::htons(port);
	addr->sin_family = AF_INET;

	*out_addrlen = sizeof(sockaddr_in);

	return true;
}


//----------------------------------------------------------------------------------------------------------------
bool NetAddress_T::FromSockaddr( sockaddr const* sa ) {
	sockaddr_in addr_in = *((sockaddr_in*) sa);

	ip4_address = addr_in.sin_addr.S_un.S_addr;
	port = ::ntohs( addr_in.sin_port );

	return true;
}


//----------------------------------------------------------------------------------------------------------------
NetAddress_T NetAddress_T::GetLocal( unsigned int serviceNumber ) {
	char myName[256];
	if( SOCKET_ERROR == ::gethostname(myName, 256) ) {
		return NetAddress_T();
	}

	char service[7];

	sprintf_s(service, "%d", serviceNumber);

	if (myName == nullptr) {
		return NetAddress_T();
	}

	// Set up search parameters
	addrinfo hints;
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;			// IPv4 address
	hints.ai_socktype = SOCK_STREAM;	// TCP socket (SOCK_DGRAM for UDP)
	hints.ai_flags = AI_PASSIVE;		// an address we can host on;
										// hints.ai_flags |= FINDADDR_NUMERIC_ONLY; // will speed up this function since it won't have to look up the address

	addrinfo* result = nullptr;
	int status = ::getaddrinfo( myName, service, &hints, &result ); // Find address info for my own host name
	if (status != 0) {
		Logger::PrintTaggedf("Net", "failed to find address for \\[%s:%s]. Error\\[%s]", myName, service, ::gai_strerror(status) );
	}

	// Result is a linked list of addresses that the search found. Iterate through and print the addresses
	addrinfo* iter = result;
	while (iter != nullptr) {

		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*) (iter->ai_addr);

			char out[256];
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 );
			//DevConsole::Printf( "Local address: %s:$u", out, ipv4->sin_port );
			return NetAddress_T((sockaddr*) ipv4);
		}

		iter = iter->ai_next;
	}

	::freeaddrinfo( result );
}


//----------------------------------------------------------------------------------------------------------------
void GetAddressExample( std::string const& command ) {

	char myName[256];
	if( SOCKET_ERROR == ::gethostname(myName, 256) ) {
		return;
	}

	char const* service = "80";

	if (myName == nullptr) {
		return;
	}

	// Set up search parameters
	addrinfo hints;
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;			// IPv4 address
	hints.ai_socktype = SOCK_STREAM;	// TCP socket (SOCK_DGRAM for UDP)
	hints.ai_flags = AI_PASSIVE;		// an address we can host on;
										// hints.ai_flags |= FINDADDR_NUMERIC_ONLY; // will speed up this function since it won't have to look up the address

	addrinfo* result = nullptr;
	int status = ::getaddrinfo( myName, service, &hints, &result ); // Find address info for my own host name
	if (status != 0) {
		Logger::PrintTaggedf("Net", "failed to find address for \\[%s:%s]. Error\\[%s]", myName, service, ::gai_strerror(status) );
	}

	// Result is a linked list of addresses that the search found. Iterate through and print the addresses
	addrinfo* iter = result;
	while (iter != nullptr) {

		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*) (iter->ai_addr);

			char out[256];
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 );
			//Logger::PrintTaggedf( "Net", "My address: %s", out );
			//DevConsole::Printf( "My address: %s", out );
		}

		iter = iter->ai_next;
	}

	::freeaddrinfo( result );
}


//----------------------------------------------------------------------------------------------------------------
bool GetAddressForHost( sockaddr* out, int* out_addrlen, char const* hostname, char const* service = "12345" ) {

	if (hostname == nullptr) {
		return false;
	}

	// Set up search parameters
	addrinfo hints;
	memset( &hints, 0, sizeof(hints) );
	hints.ai_family = AF_INET;			// IPv4 address
	hints.ai_socktype = SOCK_STREAM | SOCK_DGRAM;	// TCP socket (SOCK_DGRAM for UDP)
										// hints.ai_flags = AI_PASSIVE;		// an address we can host on;
										// hints.ai_flags |= FINDADDR_NUMERIC_ONLY; // will speed up this function since it won't have to look up the address

	addrinfo* result = nullptr;
	int status = ::getaddrinfo( hostname, service, &hints, &result ); // Find address info for my own host name
	if (status != 0) {
		return false;
	}

	// Result is a linked list of addresses that the search found. Iterate through and print the addresses
	bool foundOne = false;
	addrinfo* iter = result;
	while (iter != nullptr) {

		if (iter->ai_family == AF_INET) {
			sockaddr_in* ipv4 = (sockaddr_in*) (iter->ai_addr);

			memcpy( out, ipv4, sizeof(sockaddr_in) );
			*out_addrlen = sizeof(sockaddr_in);

			char out[256];
			inet_ntop( ipv4->sin_family, &(ipv4->sin_addr), out, 256 );
			//Logger::PrintTaggedf( "Net", "My address: %s", out );
			//DevConsole::Printf( "My address: %s", out );
			foundOne = true;
			break;
		}

		iter = iter->ai_next;
	}

	::freeaddrinfo( result );
	return foundOne;
}


//----------------------------------------------------------------------------------------------------------------
std::string NetAddress_T::to_string() const {

	std::string out = "";

	sockaddr_in addr;
	size_t size;
	ToSockaddr( (sockaddr*) &addr, &size );

	out += std::to_string(addr.sin_addr.S_un.S_un_b.s_b1);
	out += ".";
	out += std::to_string(addr.sin_addr.S_un.S_un_b.s_b2);
	out += ".";
	out += std::to_string(addr.sin_addr.S_un.S_un_b.s_b3);
	out += ".";
	out += std::to_string(addr.sin_addr.S_un.S_un_b.s_b4);
	out += ":";
	out += std::to_string( ::ntohs(addr.sin_port) );

	return out;
}


//----------------------------------------------------------------------------------------------------------------
bool NetAddress_T::operator==( NetAddress_T const& other ) const {
	if ( other.ip4_address == ip4_address && other.port == port ) {
		return true;
	} 
	return false;
}