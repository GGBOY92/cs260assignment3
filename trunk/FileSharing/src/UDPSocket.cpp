
#include "UDPSocket.hpp"


UDPSocket::UDPSocket() : iSocket()
{}


UDPSocket::UDPSocket( std::string const &name ) : iSocket( name )
{}


void UDPSocket::Init( void )
{
  socket_ = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_UDP, NULL, 0, 0 );
  if( socket_ == INVALID_SOCKET )
  {
    throw( SockErr( WSAGetLastError(), "Failure to create UDP socket." ) );
  }


  ULONG flags = 1;
  int eCode = ioctlsocket( socket_, FIONBIO, &flags ); 
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failed to set the UDP socket non-blocking" ) );
  }


  eCode = bind( socket_, reinterpret_cast<sockaddr *>( &socketAddress_ ), sizeof( socketAddress_ ) );
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failure to bind UDP socket." ) );
  }
}


void UDPSocket::InitBlocking( void )
{
  // to do: implement
}


bool UDPSocket::Receive( DataBuffer &data )
{
  return false;
}

void UDPSocket::SendTo( DataBuffer const &data, SocketAddress const &address )

void UDPSocket::Send( DataBuffer const &data )
{

}


void UDPSocket::Shutdown( void )
{

}


void UDPSocket::Close( void )
{

}
