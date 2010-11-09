/*!
 *   @File   SocketHandler.hpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Thread object for managing a socket.
 */

#include "SocketHandler.hpp"


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
    socket_.Shutdown();
  }
  catch ( Socket::SockErr e )
  {
    e.Print();
    printf( "client socket could not be shutdown\n" );
  }

  try
  {
    socket_.Close();
  }
  catch( Socket::SockErr e )
  {
    e.Print();
    printf( "Client socket could not be close.\n" );
  }

}

void SocketHandler::SendAll( void )
{
  outQueue_.Lock();
  MessageQueue &out( outQueue_.Get() );

  while( out.size() > 0 )
  {
    DataBuffer &data = out.front();

    try
    {
      socket_.Send( data );
      out.pop_front();
      Sleep( 100 );
    }
    catch( Socket::SockErr e )
    {
      e.Print();
    }
  }

  outQueue_.UnLock();
}

void SocketHandler::CollectAll( void )
{
  inQueue_.Lock();
  MessageQueue &in( inQueue_.Get() );

  try
  {
    DataBuffer data;
    while( socket_.Receive( data ) )
    {
      in.push_back( data );
    }
  }
  catch( Socket::SockErr e )
  {
    if( e.eCode_ == 0 || e.eCode_ == WSAECONNRESET )
      isDying = true;

    e.Print();
  }

  inQueue_.UnLock();
}

void SocketHandler::FlushThread( void )
{

}

SocketHandler::MessageQueue SocketHandler::PullMessages( void )
{
  inQueue_.Lock();
  MessageQueue &in( inQueue_.Get() );

  MessageQueue messages = in;
  in.clear();

  inQueue_.UnLock();

  return messages;
}

void SocketHandler::PushMessages( SocketHandler::MessageQueue const &messages )
{
  outQueue_.Lock();
  MessageQueue &out( outQueue_.Get() );

  out.insert( out.end(), messages.begin(), messages.end() );
  outQueue_.UnLock();
}

bool SocketHandler::PullMessage( DataBuffer &msg )
{
  inQueue_.Lock();
  MessageQueue &in( inQueue_.Get() );

  if( in.size() <= 0 )
  {
    inQueue_.UnLock();
    return false;
  }

  msg = in.front();
  in.pop_front();
  inQueue_.UnLock();

  return true;
}

void SocketHandler::PushMessage( DataBuffer const &msg )
{
  outQueue_.Lock();
  MessageQueue &out( outQueue_.Get() );
  out.push_back( msg );
  outQueue_.UnLock();
}
