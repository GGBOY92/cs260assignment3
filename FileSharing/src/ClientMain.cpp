/*!
 *   @File   Main.cpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  chess client main, controls the flow of the client application.
 */

#define TEST_UDP_SOCKETS 1
#define TEST_TCP_SOCKETS 0

//#include "GameClient.hpp"
#include <iostream>
#include "shared.hpp"

//testing headers

#if TEST_TCP_SOCKETS

#include "TCPSocket.hpp"

#elif TEST_UDP_SOCKETS

#include "UDPSocket.hpp"

#endif

u32 const static READ_BUFFER_SIZE = 256;

int main( int argc, char *argv[] )
{
#if TEST_TCP_SOCKETS | TEST_UDP_SOCKETS

  StartWinSock();
  
  SocketAddress remoteAddress;
  char *localIP = NULL;
  GetLocalIP( localIP );

#endif
 
#if TEST_TCP_SOCKETS

  TCPSocket client;

  remoteAddress.SetIP( localIP );
  remoteAddress.SetPortNumber( 8000 );

  client.SetIP( localIP );
  client.SetPortNumber( 8001 );

  client.Init();
  
  client.Connect( remoteAddress );

#elif TEST_UDP_SOCKETS

  UDPSocket client;

  u32 port;
  char ip[ 100 ];
  LoadConfigFile( port, ip );

  client.SetIP( localIP );
  client.SetPortNumber( port );

  remoteAddress.SetIP( ip );
  remoteAddress.SetPortNumber( port );

  client.Init();

  client.AcceptFrom( remoteAddress );

#endif

#if TEST_UDP_SOCKETS

  while( true )
  {
    NetworkMessage netMessage;

    netMessage.receiverAddress_ = remoteAddress;
    
    std::string message;
    Prompt( message, "Enter a message." );

    netMessage.msg_.Assign( message.c_str(), message.size() );

    client.Send( netMessage );

    while( true )
    {
      if( client.Receive( netMessage ) )
      {
        printf( netMessage.msg_.Bytes() );
        break;
      }
    }
  }

#endif

#if TEST_TCP_SOCKETS

  while( true )
  {
    NetworkMessage netMessage;

    if( client.Receive( netMessage ) )
    {
      printf( netMessage.msg_.Bytes() );
    }
  }

#endif

#if TEST_TCP_SOCKETS | TEST_UDP_SOCKETS

  client.Shutdown();
  client.Close();

  CloseWinSock();

#endif

  /*
  GameClient client;

  try
  {
    client.Init();

    try
    {
      try
      {
        client.Run();
      }
      catch( BaseErrExcep e )
      {
        e.Print();
      }
    }
    catch( BaseErrExcep e )
    {
      e.Print();
    }

    client.Close();
  }
  catch( BaseErrExcep e )
  {
    e.Print();
  }
  */

  return WaitForInput();
}
