/*!
 *   @File   SocketHandler.hpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Thread object for managing a socket.
 */

#pragma once

#include <string>
#include <deque>

#include "TCPSocket.hpp"
#include "SocketHandler.hpp"

class TCPSocketHandler : public SocketHandler
{
public: // classes
  
public: // methods

  TCPSocketHandler( TCPSocket &socket, u32 conID );

  /*
  virtual void InitThread( void );
	virtual void Run( void );
	virtual void FlushThread( void );
  */

  u32 GetConID( void ) const { return conID_; }
  void SetConID( u32 conID ) { conID_ = conID; }
  
private: // methods

private: // members

  TCPSocket socket_;
  u32 conID_;

};
