/*!
 *   @File   Main.cpp
 *   @Author Steven Liss
 *   @Date   30 Sept 2010
 *   @Brief  chess server main, controls the flow of the server application.
 */

#include "shared.hpp"
#include "Server.hpp"
//#include "GameControl.hpp"
#include "FileServer.hpp"

int main( int argc, char *argv[] )
{
#ifdef TEST_TCP_SOCKETS

  StartWinSock();

  TCPSocket listener;
  TCPSocket client;

  char *localIP = NULL;
  GetLocalIP( localIP );

  listener.SetIP( localIP );
  listener.SetPortNumber( 8000 );

  listener.Init();
  
  listener.Listen();

  while( !listener.Accept( client ) );
  
  while( true )
  {
    std::string message;
    Prompt( message, "Enter a message" );

    NetworkMessage netMessage;
    netMessage.msg_.Assign( message.c_str(), message.size() );

    client.Send( netMessage );
  }

  client.Shutdown();
  client.Close();

  listener.Shutdown();
  listener.Close();

  CloseWinSock();

#endif

  /*
  GameControl controller;

  try
  {
    controller.Init();

    try
    {
      controller.Run();
    }
    catch( BaseErrExcep e )
    {
      e.Print();
    }

    controller.Close();
  }
  catch( BaseErrExcep e )
  {
    e.Print();
  }
  */

    FileServer fileServer;

    try
    {
        fileServer.Init();

        try
        {
            fileServer.Run();
        }
        catch(BaseErrExcep& e)
        {
            e.Print();
        }

        fileServer.Close();
    }
    catch(BaseErrExcep& e)
    {
        e.Print();
    }
 
   
  printf( "\n Server is closing \n" );

  return WaitForInput();
}
