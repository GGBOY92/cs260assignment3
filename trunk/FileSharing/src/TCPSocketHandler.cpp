/*!
 *   @File   SocketHandler.hpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Thread object for managing a socket.
 */

#include "TCPSocketHandler.hpp"

TCPSocketHandler::TCPSocketHandler( TCPSocket &socket, u32 conID ) :
    SocketHandler( &socket_ ), socket_( socket ), conID_( conID )
{

}

void TCPSocketHandler::OnRun( void )
{}
