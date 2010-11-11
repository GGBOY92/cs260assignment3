
#pragma once

#include "UDPSocket.hpp"
#include "ActiveObject.hpp"

class UDPSocketHandler
{
public: // methods

  void AddTransfer( SocketAddress const &address );

private: // members

  UDPSocket socket_;

};

void UDPSocketHandler::AddTransfer( SocketAddress const &address )
{
  socket_.ExpectFrom( address );
}
