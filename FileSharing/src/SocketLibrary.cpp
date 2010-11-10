/*!
 *   @File   SocketLibrary.cpp
 *   @Author Steven Liss
 *   @Date   24 Sept 2010
 *   @Brief  A collection of routines and data for global winsock operations.
 *   @Note   The data and routines in this file should be collected into a single class.
 */

#include "SocketLibrary.hpp"

#include <cstdio>

void StartWinSock( void )
{
  int eCode = 0;

  WSAData dat;
  SecureZeroMemory( &dat, sizeof(dat) );

  eCode = WSAStartup( MAKEWORD( 2, 2 ), &dat );
  
  if( eCode != 0 )
  {
    throw( WSErr( eCode, "Failure to initialize WinSock." ) );
  }
  else
  {
    ShowWinSockVersion( dat );
  }
}

void CloseWinSock( void )
{
  int eCode = WSACleanup();
  if( eCode != 0 )
    throw( WSErr( eCode, "Failure to close WinSock." ) );
}

void ShowWinSockVersion( WSAData const &dat )
{
  int majVer = LOBYTE( dat.wVersion );
  int minVer = HIBYTE( dat.wVersion );

  printf( "WinSock version           - %d.%d\n", majVer, minVer );

  majVer = LOBYTE( dat.wHighVersion );
  minVer = HIBYTE( dat.wHighVersion );

  printf( "Highest available version - %d.%d\n", majVer, minVer );
}

void GetLocalIP( char * &localIP )
{
  hostent* localHost;
  localHost = gethostbyname("");
  in_addr **in = reinterpret_cast< in_addr ** >( localHost->h_addr_list );
  localIP = inet_ntoa( **in );
}

WinSockErrExcep::WinSockErrExcep( int eCode, char const *msg )
  : BaseErrExcep( msg ), eCode_( eCode )
{

}

WinSockErrExcep::WinSockErrExcep( int eCode, std::string const &msg )
  : BaseErrExcep( msg ), eCode_( eCode )
{

}

void WinSockErrExcep::Print( void )
{
  printf( msg_.c_str() );
  printf( "  with error code - %d\n", eCode_ );
}

u32 MessageHeader::WriteMessageHeader( char *buffer )
{
  memcpy( buffer, cSize_, sizeof( u32 ) );
  return sizeof( u32 );
}

u32 MessageHeader::ReadMessageHeader( char *buffer )
{
  size_ = 0;
  memcpy( cSize_, buffer, sizeof( u32 ) );
  return sizeof( u32 );
}