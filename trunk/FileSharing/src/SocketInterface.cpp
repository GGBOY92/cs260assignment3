#include "SocketInterface.hpp"

void iSocket::SetIP( char *IP )
{
  socketAddress_.sin_addr.s_addr = inet_addr( IP );
}

void iSocket::SetPortNumber( u32 port )
{
  socketAddress_.sin_port = htons( port );
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

u32 iSocket::MessageHeader::WriteMessageHeader( char *buffer )
{
  memcpy( buffer, cSize_, sizeof( u32 ) );
  return sizeof( u32 );
}

u32 iSocket::MessageHeader::ReadMessageHeader( char *buffer )
{
  msgSize_ = 0;
  memcpy( cSize_, buffer, sizeof( u32 ) );
  return sizeof( u32 );
}
