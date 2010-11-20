/*!
 *   @File   Main.cpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  chess client main, controls the flow of the client application.
 */

#define TEST_UDP_SOCKETS 0
#define TEST_TCP_SOCKETS 0
#define TEST_FILE_SPLITTER 1

#include "FileClient.hpp"
#include "shared.hpp"
#include "SocketLibrary.hpp"

#include "Splitter.hpp"
#include "IOThread.hpp"

//testing headers

#if TEST_TCP_SOCKETS

#include "TCPSocket.hpp"

#elif TEST_UDP_SOCKETS

#include "UDPSocket.hpp"

#elif TEST_FILE_SPLITTER

#include "File.hpp"

#endif

#include <iostream>

u32 const static READ_BUFFER_SIZE = 256;

int main( int argc, char *argv[] )
{
    
/*
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow )
{
    return main( 0, NULL );
}
*/

    FileClient client;

    try
    {
        try
        {
             // read config.ini, init winsock, set remote addr
            client.Init();
            try
            {
                 // establish connection with file server
                client.ConnectToServer();
                 // send the server the initial list of files client has available
                client.SendFileList();

                 // process incomin messages and user input
                  // run until connection is lost or user quits
                client.Run();
            }
            catch(iSocket::SockErr& e)
            {
                e.Print();
            }
        }
        catch(WSErr& e)
        {
            e.Print();
        }

        client.Close();
    }
    catch(iSocket::SockErr& e)
    {
        e.Print();
    }

   
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
        printf( "\n" );
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

  FileSplitter splitter;
  splitter.SetDirectory( "C:\\splitter_test\\" );
  splitter.SetFilename( "split_target.mp3" );
  splitter.SetChunkSize( 800 );

  std::vector< iFileInfo::Chunk > chunks;
  
  iFileInfo::Chunk chunk;
  while( splitter.GetNextChunk( chunk ) )
  {
    chunks.push_back( chunk );
  }


  FileJoiner joiner;

  joiner.SetDirectory( "C:\\splitter_test\\" );
  joiner.SetFilename( "join_target.mp3" );
  joiner.SetFileSize( splitter.GetFileSize() );
  joiner.SetChunkSize( 800 );

  for( u32 i = 0; i < chunks.size(); ++i )
  {
    if( joiner.PutChunk( chunks[ i ] ) )
      printf( "file complete\n" );
    else
      printf( "file incomplete\n" );
  }

  */

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
