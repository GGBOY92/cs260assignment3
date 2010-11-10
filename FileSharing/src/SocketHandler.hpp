/*!
 *   @File   SocketHandler.hpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Thread object for managing a socket.
 */

#pragma once

#include <string>
#include <deque>

#include "Socket.hpp"
#include "ActiveObject.hpp"
#include "SecureObject.hpp"

class SocketHandler : public ActiveObject
{
public:
  typedef std::deque< DataBuffer > MessageQueue; 

private:
  Socket socket_;
  u32 conID_;

  SecureObject< MessageQueue > inQueue_;
  SecureObject< MessageQueue > outQueue_;

public:

  SocketHandler( Socket const &socket, u32 conID ) :
      ActiveObject(), socket_( socket ), conID_( conID ), inQueue_( MessageQueue() ), outQueue_( MessageQueue() )
  {}
	
  virtual void InitThread( void );
	virtual void Run( void );
	virtual void FlushThread( void );

  u32 GetConID( void ) const { return conID_; }
  void SetConID( u32 conID ) { conID_ = conID; }
  
  void SendAll( void );
  void CollectAll( void );
  SocketHandler::MessageQueue PullMessages( void );
  void PushMessages( SocketHandler::MessageQueue const &messages );
  
  bool PullMessage( DataBuffer &msg );
  void PushMessage( DataBuffer const &msg );
  bool IsDying( void ) { return isDying; }
};
