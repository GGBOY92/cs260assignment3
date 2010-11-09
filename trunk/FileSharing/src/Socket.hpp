/*!
 *   @File   Socket.hpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  A collection of routines and data for controlling socket operations
 */

#pragma once

#include "SocketLibrary.hpp"
#include <string>

class Socket
{
  sockaddr_in socketAddress_;
  SOCKET socket_;
  std::string name_;
  bool disconnect_;

public:
  Socket();
  Socket( std::string const &name );
  void Init( void );
  void InitBlocking( void );
  bool Connect( Socket const &remoteSock );
  bool Connect( SocketAddress const &remoteSock );
  void Close( void );

  bool Accept( Socket &endPoint );
  void Listen( void );

//  bool Receive( std::string &data );
  bool Receive( DataBuffer &data );
//  void Send( std::string const &data );
  void Send( DataBuffer const &data );
  void Shutdown( void );

  void SetIP( char *IP );
  void SetPortNumber( u32 port );

  struct SocketErrExcep : public WSErr
  {
    SocketErrExcep( int eCode, char const *msg, std::string const &socketName = std::string() );
    SocketErrExcep( int eCode, std::string const &msg, std::string const &socketName = std::string() );
    void Print( void );

    // data
    std::string socketName_;
  };

  typedef SocketErrExcep SockErr;

private:
  static u32 const MAX_CONN_QUEUE;
  static u32 const SEND_BUFFER_SIZE;
  static u32 const RECV_BUFFER_SIZE;
};
