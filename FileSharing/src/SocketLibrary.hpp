/*!
 *   @File   SocketLibrary.hpp
 *   @Author Steven Liss
 *   @Date   24 Sept 2010
 *   @Brief  A collection of routines and data for global winsock operations.
 *   @Note   The data and routines in this file should be collected into a single class.
 */

#pragma once

#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

#include "shared.hpp"

std::string const gTerminate = "quit";
static u32 const MAX_INPUT_SIZE = 1000;

struct WinSockErrExcep : public BaseErrExcep
{
  WinSockErrExcep( int eCode, char const *msg );
  WinSockErrExcep( int eCode, std::string const &msg );

  void Print( void );

  //data
  int eCode_;
};

typedef WinSockErrExcep WSErr;

struct SocketAddress
{
  SocketAddress()
  {
    SecureZeroMemory( &adr_, sizeof( adr_ ) );
    adr_.sin_family = AF_INET;
  }

  SocketAddress( sockaddr_in adr ) : adr_( adr ) {}
  
  void SetPortNumber( u32 port )
  {
    adr_.sin_port = htons( port );
  }

  void SetIP( std::string IP )
  {
    adr_.sin_addr.s_addr = inet_addr( IP.c_str() );
  }

  bool operator ==( SocketAddress const &rhs )
  {
    if( adr_.sin_port != rhs.adr_.sin_port )
      return false;

    if( adr_.sin_addr.s_addr != rhs.adr_.sin_addr.s_addr )
      return false;

    return true;
  }

  sockaddr_in adr_;
};

typedef SocketAddress SockAddr;


struct NetworkMessage
{
  enum
  {
    DISCON = 1000,
    JOIN,
    SERVER_FILES,
    REQ_FILES,
    QUIT,
    TRANSFER,
    INFORM_SENDER,
    INFORM_RECEIVER,
	  ERR,

    // always at bottom
    SIZE
  };

  NetworkMessage() :
    msg_(), conID_( -1 ), type_( -1 )
  {}

  DataBuffer msg_;
  int conID_;
  MsgType type_;
  SocketAddress receiverAddress_;
  void operator<<( DataBuffer const &data )
  {
      if( data.Size() == 0 )
      {
          type_ = SIZE;
          return;
      }

      
      u32 sizeType = sizeof( MsgType );
      memcpy( &( type_ ), data.Bytes(), sizeType );
      msg_.Assign( data.Bytes( sizeType ), data.Size() - sizeType );
  }

  template < typename T >
  void operator>>( T &t )
  {
    u32 sizeType = sizeof( MsgType );
    t.type_ = type_;

    unsigned sizeOfData = sizeof( T::Data );
    memcpy( (char *)&t.data_, msg_.Bytes(), sizeOfData );
  }


  template < typename T >
  void operator<<( T const &t )
  {
    u32 sizeType = sizeof( MsgType );
    type_ = t.type_;
    unsigned sizeOfData = sizeof( T::Data );
    msg_.Assign( (const char *)&t.data_, sizeOfData );
  }
};

typedef NetworkMessage NetMsg;

void StartWinSock( void );
void CloseWinSock( void );
void ShowWinSockVersion( WSAData const &dat );
void GetLocalIP( char* &localIP );
