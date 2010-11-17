
#pragma once

#include "UDPSocket.hpp"
#include "SocketHandler.hpp"

#include <deque>

class UDPSocketHandler : public SocketHandler
{

public: // methods

  UDPSocketHandler( UDPSocket &socket ) :
      SocketHandler( &socket_ ), socket_( socket )
  {}

  void AddTransfer( SocketAddress const &address );

protected: // methods
  
  virtual void OnRun( void );

private: // members
  
  UDPSocket socket_;

};
