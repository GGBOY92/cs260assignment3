/*!
 *   @File   NetworkClient.cpp
 *   @Author Steven Liss
 *   @Date   16 Oct 2010
 *   @Brief  Client side network functionality
 */

#include "NetworkClient.hpp"

NetworkClient::NetworkClient( void )
{}

void NetworkClient::Init( void )
{
  connected_ = false;
    //try to init WinSock
  try
  {
    StartWinSock();
  }
  catch( WSErr e )
  {
    e.Print();
    throw( BaseErrExcep( "The server failed because winsock could not be initialized.\n" ) );
  }

  char *localIP = NULL;
  char remoteIP[16];
  u32 port;

  LoadConfigFile( port, remoteIP );

  // debug code
  //GetLocalIP( remoteIP );
  // end debug code

  GetLocalIP( localIP );

  local_.SetPortNumber( port );
  remote_.SetPortNumber( port );

  local_.SetIP( localIP );
  remote_.SetIP( remoteIP );
  
  //remote_.SetIP( "192.168.1.8" );
  //remote_.SetIP( "10.11.49.20" );

  try
  {
    //local_.InitBlocking();
    local_.Init();
  }
  catch( Socket::SockErr e )
  {
    e.Print();
    throw( BaseErrExcep( "The server's listener socket could not be initialized.\n" ) );
  }
}

void NetworkClient::Connect( void )
{
  while( !connected_ )
  {
    try
    {
      local_.Connect( remote_ );
      connected_ = true;
    }
    catch( Socket::SockErr e )
    {
      e.Print();
      printf( "Sockets could not be connected." );
      if( PromptYesNo( "\nWould you like to try again?" ) )
        continue;
      else
        throw( BaseErrExcep( "Connection failure" ) );
    }
  }
}

void NetworkClient::Update( void )
{
  while( true )
  {
    std::string inBuffer = "";
    Prompt( inBuffer, "Enter text to send" );

    DataBuffer data( inBuffer.c_str(), inBuffer.size() );

    local_.Send( data );
    
    if( inBuffer == gTerminate )
      break;

    DataBuffer echoBuffer;
    local_.Receive( echoBuffer );
    printf( "echo: %s\n", echoBuffer.Bytes() );
  }

  try
  {
    Sleep( 5 );
    local_.Shutdown();
  }
  catch( Socket::SockErr e )
  {
    e.Print();
    printf( "Failed to shutdown local socket.\n" );
  }
}

void NetworkClient::Close( void )
{
  try
  {
    local_.Close();
  }
  catch( Socket::SockErr e )
  {
    e.Print();
    printf( "Local socket could not be closed\n" );
  }

  try
  {
    CloseWinSock(); 
  }
  catch( WSErr e )
  {
    e.Print();
    throw( BaseErrExcep( "The server failed to close properly because winsock could not close" ) );
  }
}

void NetworkClient::SendMessage( NetworkMessage const &msg )
{
  if( !connected_ )
	  return;

  try
  {
    DataBuffer data;
    data.Assign( ( char * ) &msg.type_, sizeof( MsgType ) );
    data.Append( msg.msg_.Bytes(), msg.msg_.Size() );
    local_.Send( data );
  }
  catch( Socket::SockErr e )
  {
    e.Print();
  }
}

void NetworkClient::WaitForMessage( NetworkMessage &rMsg )
{
  if( !connected_ )
    return;

  try
  {
    DataBuffer msg;

    while( !local_.Receive( msg ) );

    rMsg << msg;
  }
  catch( Socket::SockErr e )
  {
    e.Print();
  }
}

bool NetworkClient::CheckForMessage( NetMsg &nMsg )
{
  if( !connected_ )
    return false;

  try
  {
    DataBuffer msg;
    if( local_.Receive( msg ) )
    {
      nMsg << msg;
      return true;
    }
    else
      return false;
  }
  catch( Socket::SockErr e )
  {
    if(e.eCode_ == 0 || e.eCode_ == WSAECONNRESET )
		connected_ = false;

    e.Print();
    return false;
  }
}
