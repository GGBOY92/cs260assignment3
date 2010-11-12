
#include "UDPSocket.hpp"

#include <algorithm>


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
  char buffer[ UDP_PACKET_SIZE ];

  SocketAddress address;
  int size = sizeof( address.adr_ );
  int eCode = recvfrom( socket_, buffer, UDP_PACKET_SIZE, 0, ( sockaddr * ) &address.adr_, &size );
  ReceiveUntil( buffer, UDP_PACKET_SIZE, UDP_PACKET_SIZE, 0 );

  MsgHdr header;
  header.ReadMessageHeader( buffer );
  

  
  return true;
}


void UDPSocket::SendTo( DataBuffer const &data, SocketAddress const &address )
{
  if( data.Size() + MsgHdr::GetSize() > UDP_PACKET_SIZE )
    printf( "packet truncation in UDPSokcet::SendTo\n" );

  u32 bytesSent = 0;
  u32 totalBytesSent = 0;

  char buffer[ UDP_PACKET_SIZE ];

  MsgHdr header;
  header.msgSize_ = data.Size();

  header.WriteMessageHeader( buffer );

  while( totalBytesSent < UDP_PACKET_SIZE )
  {
    sockaddr *pSAddr = ( sockaddr * ) &address.adr_;
    int size = sizeof( address.adr_ );
    int eCode = sendto( socket_, data.Bytes(), UDP_PACKET_SIZE, 0, pSAddr, size );

    if( eCode == SOCKET_ERROR )
    {
      eCode = WSAGetLastError();
      if( eCode == WSAEWOULDBLOCK )
        continue;

      throw( SockErr( eCode, "SendTo generated an error\n" ) );
    }
    else
    {
      bytesSent = eCode;
    }

    totalBytesSent += bytesSent;
  }
}


void UDPSocket::Send( DataBuffer const &data )
{

}


void UDPSocket::Shutdown( void )
{

}


void UDPSocket::Close( void )
{

}

void UDPSocket::ExpectFrom( SocketAddress const & address )
{
  senders.push_back( address );
}

bool UDPSocket::ValidSender( SocketAddress const &address )
{
  AddressList::iterator addIt = std::find( senders.begin(), senders.end(), address );

  if( addIt == senders.end() )
    return false;
  else
    return true;
}

void UDPSocket::UDPMessageHeader::WriteMessageHeader( char *buffer )
{
  memcpy( buffer, cData_, GetSize() );
}

void UDPSocket::UDPMessageHeader::ReadMessageHeader( char const *buffer )
{
  memcpy( cData_, buffer, GetSize() );
}
