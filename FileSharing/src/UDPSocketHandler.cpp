#include "UDPSocketHandler.hpp"


void UDPSocketHandler::AddTransfer( SocketAddress const &address )
{
  socket_.AcceptFrom( address );
}

