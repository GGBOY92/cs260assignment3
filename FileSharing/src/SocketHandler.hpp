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

class TCPSocketHandler : public ActiveObject
{
public: // classes
  typedef std::deque< DataBuffer > MessageQueue; 

public: // methods

  TCPSocketHandler( TCPSocket &socket, u32 conID ) :
      ActiveObject(), socket_( socket ), conID_( conID ), inQueue_( MessageQueue() ), outQueue_( MessageQueue() )
  {}
	
  virtual void InitThread( void );
	virtual void Run( void );
	virtual void FlushThread( void );

  u32 GetConID( void ) const { return conID_; }
  void SetConID( u32 conID ) { conID_ = conID; }
  
  TCPSocketHandler::MessageQueue PullMessages( void );
  void PushMessages( TCPSocketHandler::MessageQueue const &messages );
  
  bool PullMessage( DataBuffer &msg );
  void PushMessage( DataBuffer const &msg );
  bool IsDying( void ) { return isDying; }

private: // methods

  void SendAll( void );
  void CollectAll( void );

private: // members

  TCPSocket socket_;
  u32 conID_;

  SecureObject< MessageQueue > inQueue_;
  SecureObject< MessageQueue > outQueue_;

};
