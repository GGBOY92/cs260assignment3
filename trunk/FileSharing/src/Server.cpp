/*!
 *   @File   Server.cpp
 *   @Author Steven Liss
 *   @Date   14 Oct 2010
 *   @Brief  Server class for managing multiple connections
 */

#include "Server.hpp"
#include "TCPSocketHandler.hpp"

void Server::Init( void )
{
  conCount_ = 0;

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
  GetLocalIP( localIP );

  Config serverConfig;
  serverConfig.LoadConfigFile();

  listener_.SetPortNumber( serverConfig.serverPort_ );
  listener_.SetIP( localIP );

  try
  {
    listener_.Init();
  }
  catch( iSocket::SockErr e )
  {
    CloseWinSock();

    e.Print();
    throw( BaseErrExcep("The server's listener socket could not be initialized.\n") );
  }

  try
  {
    listener_.Listen();
    printf("Server is listening for connections...\nPort: %d\nIP: %s\n", serverConfig.serverPort_, localIP);
  }
  catch( iSocket::SockErr e )
  {
    e.Print();
  }
}


void Server::Update( void )
{
  TCPSocket client;
  if( CheckForNewClient( client ) )
  {
    AddNewHandler( client );
  }

  PullAllMessages();
  PushAllMessages();

  RemoveDeadHandlers();
}


void Server::Run( void )
{
  bool wait = true;

  try
  {
    while( wait )
    {
      Update();
    }
  }
  catch( BaseErrExcep e )
  {
    e.Print();
    wait = PromptYesNo( "The server encountered an error while running, would you like to continue?\n" );
  }
}


void Server::Close( void )
{
  try
  {
    listener_.Shutdown();
  }
  catch ( iSocket::SockErr e )
  {
    e.Print();
    printf( "listener socket could not be shutdown\n" );
  }

  try
  {
    listener_.Close();
  }
  catch( iSocket::SockErr e )
  {
    e.Print();
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


bool Server::CheckForNewClient( TCPSocket &rSocket )
{
  try
  {
    return listener_.Accept( rSocket );
  }
  catch( iSocket::SockErr e )
  {
    e.Print();
    throw( BaseErrExcep( "Listener could not accept a client.\n" ) );
  }
}


/*
TCPSocket *Server::CheckForNewClient( )
{
  try
  {
    return listener_.Accept();
  }
  catch( iSocket::SockErr e )
  {
    e.Print();
    throw( BaseErrExcep( "Listener could not accept a client.\n" ) );
  }
}
*/

void Server::WaitForNewClient( TCPSocket &rSocket )
{
  try
  {
    bool noConnect = true;

    while( noConnect )
    {
      noConnect = listener_.Accept( rSocket );
    }
  }
  catch( iSocket::SockErr e )
  {
    e.Print();
    throw( BaseErrExcep( "Listener could not accept the client.\n" ) );
  }
}


void Server::AddNewHandler( TCPSocket &sock )
{
  TCPSocketHandler *handler = new TCPSocketHandler( sock, conCount_ );
  ++conCount_;
  connections_.push_back( handler );
  handler->Wake();
}


void Server::RemoveDeadHandlers( void )
{
  for( ConnectionList::iterator conIt = connections_.begin(); conIt != connections_.end(); )
  {
    TCPSocketHandler *sock = *conIt;
    if( sock->IsDying() )
    {
      sock->Kill();

      NetMsg nMsg;
      nMsg.type_ = NetMsg::DISCON;
      nMsg.conID_ = sock->GetConID();
      inQueue_.push_back( nMsg );
      conIt = connections_.erase( conIt );
    }
    else
    {
      ++conIt;
    }
  }
}


void Server::PullAllMessages( void )
{
  for( u32 i = 0; i < connections_.size(); ++i )
  {
    TCPSocketHandler *clientHandler = connections_[ i ];

    if( clientHandler->IsDying() )
      continue;

    NetworkMessage msg;
    while( clientHandler->PullMessage( msg ) )
    {
      msg.conID_ = clientHandler->GetConID();

      inQueue_.push_back( msg );
    }
  }
}


void Server::PushAllMessages( void )
{
  while( outQueue_.size() )
  {
    NetworkMessage nMsg = outQueue_.front();
    outQueue_.pop_front();

    TCPSocketHandler *clientHandler = GetHandler( nMsg.conID_ );
    
    if( clientHandler )
    {
      clientHandler->PushMessage( nMsg );
    }
  }
}


void Server::GetMessages( std::deque< NetworkMessage > &messages )
{
  while( inQueue_.size() )
  {
    messages.push_back( PopInQueue() );
  }
}


bool Server::GetMessage( NetworkMessage &msg )
{
  if( inQueue_.size() <= 0 )
    return false;

  msg = PopInQueue();
  return true;
}


NetworkMessage Server::PopInQueue()
{
  NetworkMessage netMsg = inQueue_.front();
  inQueue_.pop_front();
  return netMsg;
}


void Server::SendMessage( NetworkMessage const &msg )
{
  outQueue_.push_back( msg );
}


TCPSocketHandler *Server::GetHandler( u32 conID )
{
  for( u32 i = 0; i < connections_.size(); ++i )
  {
    TCPSocketHandler *pSockHndlr = connections_[ i ];
    if( pSockHndlr->GetConID() == conID )
    {
      return pSockHndlr;
    }
  }

  return NULL;
}
