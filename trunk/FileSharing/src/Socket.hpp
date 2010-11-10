/*!
 *   @File   Socket.hpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  A collection of routines and data for controlling socket operations
 */

#pragma once

#include "SocketInterface.hpp"
#include <string>

class Socket : public iSocket
{
public:
  Socket();
  Socket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );
  bool Connect( Socket const &remoteSock );
  virtual bool Connect( SocketAddress const &remoteSock );
  virtual void Close( void );

  virtual bool Accept( iSocket &endPoint );
  virtual void Listen( void );
  virtual bool Receive( DataBuffer &data );
  virtual void Send( DataBuffer const &data );
  virtual void Shutdown( void );
};
