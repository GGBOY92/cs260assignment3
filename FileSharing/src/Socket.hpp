/*!
 *   @File   Socket.hpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  A collection of routines and data for controlling socket operations
 */

#pragma once

#include "SocketInterface.hpp"
#include <string>

class TCPSocket : public iSocket
{
public:
  TCPSocket();
  TCPSocket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );

  void Listen( void );
  TCPSocket *Accept( void );
  bool Connect( TCPSocket const &remoteSock );
  bool Connect( SocketAddress const &remoteSock );

  // data functions
  virtual bool Receive( DataBuffer &data );
  virtual void Send( DataBuffer const &data );
  
  // clean up functions
  virtual void Shutdown( void );
  virtual void Close( void );
};
