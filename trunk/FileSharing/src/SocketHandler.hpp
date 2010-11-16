
#pragma once

#include "ActiveObject.hpp"
#include "SecureObject.hpp"
#include "SocketInterface.hpp"

#include <deque>

class SocketHandler : public ActiveObject
{

public: // classes
  
  typedef std::deque< NetworkMessage > MessageQueue; 

public: // methods

  SocketHandler( iSocket *pSocket );

  virtual void InitThread( void );
	virtual void Run( void );
	virtual void FlushThread( void );

  bool PullMessage( NetworkMessage &msg );
  void PushMessage( NetworkMessage const &msg );

  /*

  deprecated

  MessageQueue PullMessages( void );
  void PushMessages( MessageQueue const &messages );
  */

  bool IsDying( void ) { return isDying; }

protected: // methods

  void SendAll( void );
  void CollectAll( void );

protected: // members
  
  SecureObject< MessageQueue > inQueue_;
  SecureObject< MessageQueue > outQueue_;

private: // members

  iSocket *pSocket_;
};

