#include "SocketInterface.hpp"


void iSocket::SetIP( char *IP )
{
  socketAddress_.sin_addr.s_addr = inet_addr( IP );
}


void iSocket::SetPortNumber( u32 port )
{
  socketAddress_.sin_port = htons( port );
}


void iSocket::Shutdown( void )
{
  Sleep( 100 );
  int eCode = shutdown( socket_, SD_BOTH );

  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failed to shutdown on sending socket." ) );
  }
}


void iSocket::Close( void )
{
  int eCode = closesocket( socket_ );

  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failure to close socket" ) );
  }
}


bool iSocket::ReceiveUntil( char *buffer, u32 recvCount, u32 bufferSize, u32 bufferOffset /* = 0 */ )
{
  u32 writeCount = bufferSize - bufferOffset;
  if( recvCount > writeCount )
    // to do: throw something
    return false;

  buffer += bufferOffset;
  u32 totalBytesRead = 0;
  u32 bytesRead = 0;

  while( totalBytesRead < recvCount )
  {
    int eCode = recv( socket_, buffer, writeCount, 0 );
  
    if( eCode == 0 )
    {
      disconnect_ = true;
      throw( SockErr( 0, "Remote end point has shutdown the connection." ) );
    }
    else if( eCode == SOCKET_ERROR )
    {
      eCode = WSAGetLastError();

      if( eCode == WSAEWOULDBLOCK )
        return false;

      throw( SockErr( WSAGetLastError(), "Failure to receive." ) );
    }

    bytesRead = eCode;
    totalBytesRead += bytesRead;
    buffer += bytesRead;
    writeCount -= bytesRead;
  }

/*
  if( totalBytesRead > recvCount )
    return false;
  else if( totalBytesRead < recvCount )
    // to do: this is bad, throw something
    return false;
  else
    return true;
*/

  return true;
}


iSocket::SocketErrExcep::SocketErrExcep( int eCode, char const *msg, std::string const &socketName )
  : WinSockErrExcep( eCode, msg_ ), socketName_( socketName )
{

}


iSocket::SocketErrExcep::SocketErrExcep( int eCode, std::string const &msg, std::string const &socketName )
  : WinSockErrExcep( eCode, msg ), socketName_( socketName )
{

}


void iSocket::SocketErrExcep::Print( void )
{
  if( socketName_ == "" )
  {
    printf("A socket generated an error.  ");
  }
  else
  {
    printf("Socket %s generated an error.  ", socketName_.c_str() );
  }

  WinSockErrExcep::Print();
}
