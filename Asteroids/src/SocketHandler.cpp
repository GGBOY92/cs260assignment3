#include "SocketHandler.hpp"

SocketHandler::SocketHandler( iSocket *pSocket ) : pSocket_( pSocket ), inQueue_( MessageQueue() ), outQueue_( MessageQueue() )
{

}


void SocketHandler::InitThread( void )
{

}


void SocketHandler::Run( void )
{
  while( !isDying )
  {
    SendAll();
    CollectAll();
  }

  try
  {
    pSocket_->Shutdown();
  }
  catch ( iSocket::SockErr e )
  {
    e.Print();
    printf( "client socket could not be shutdown\n" );
  }

  try
  {
    pSocket_->Close();
  }
  catch( iSocket::SockErr e )
  {
    e.Print();
    printf( "Client socket could not be close.\n" );
  }

}


void SocketHandler::SendAll( void )
{
  outQueue_.Lock();
  MessageQueue &out( outQueue_.Access() );

  while( out.size() > 0 )
  {
    NetworkMessage &netMessage = out.front();

    try
    {
      pSocket_->Send( netMessage );
      out.pop_front();
      Sleep( 100 );
    }
    catch( iSocket::SockErr e )
    {
      e.Print();
    }
  }

  outQueue_.Unlock();
}


void SocketHandler::CollectAll( void )
{
  inQueue_.Lock();
  MessageQueue &in( inQueue_.Access() );

  try
  {
    NetworkMessage netMessage;
    while( pSocket_->Receive( netMessage ) )
    {
      in.push_back( netMessage );
    }
  }
  catch( iSocket::SockErr e )
  {
    if( e.eCode_ == 0 || e.eCode_ == WSAECONNRESET )
      isDying = true;

    e.Print();
  }

  inQueue_.Unlock();
}


void SocketHandler::FlushThread( void )
{

}


bool SocketHandler::PullMessage( NetworkMessage &msg )
{
  inQueue_.Lock();
  MessageQueue &in( inQueue_.Access() );

  if( in.size() <= 0 )
  {
    inQueue_.Unlock();
    return false;
  }

  msg = in.front();
  in.pop_front();
  inQueue_.Unlock();

  return true;
}


void SocketHandler::PushMessage( NetworkMessage const &msg )
{
  outQueue_.Lock();
  MessageQueue &out( outQueue_.Access() );
  out.push_back( msg );
  outQueue_.Unlock();
}

//
// not supported
//
///*
// *  same as PullMessage but returns all messages at once
// */
//SocketHandler::MessageQueue SocketHandler::PullMessages( void )
//{
//  inQueue_.Lock();
//  MessageQueue &in( inQueue_.Access() );
//
//  MessageQueue messages = in;
//  in.clear();
//
//  inQueue_.Unlock();
//
//  return messages;
//}
//
//
///*
// *  same as PushMessage but pushes all messages in messages
// */
//void SocketHandler::PushMessages( SocketHandler::MessageQueue const &messages )
//{
//  outQueue_.Lock();
//  MessageQueue &out( outQueue_.Access() );
//
//  out.insert( out.end(), messages.begin(), messages.end() );
//  outQueue_.Unlock();
//}
//
