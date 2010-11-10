/*!
 *   @File   Socket.cpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  A collection of routines and data for controlling socket operations
 */

#include "Socket.hpp"

/*
u32 const Socket::MAX_CONN_QUEUE = 10;
u32 const Socket::SEND_BUFFER_SIZE = 1000;
u32 const Socket::RECV_BUFFER_SIZE = 1000;
*/

TCPSocket::TCPSocket() : iSocket()
{
  socketAddress_.sin_family = AF_INET;
}

TCPSocket::TCPSocket( std::string const &name ) : iSocket( name )
{
  socketAddress_.sin_family = AF_INET;
}

void TCPSocket::Init( void )
{
  socket_ = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0 );
  
  if( socket_ == INVALID_SOCKET )
  {
    throw( SockErr( WSAGetLastError(), "Failure to create socket." ) );
  }


  ULONG flags = 1;
  int eCode = ioctlsocket( socket_, FIONBIO, &flags ); 
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failed to set the socket non-blocking" ) );
  }


  eCode = bind( socket_, reinterpret_cast<sockaddr *>( &socketAddress_ ), sizeof( socketAddress_ ) );
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failure to bind socket." ) );
  }
}

void TCPSocket::InitBlocking( void )
{
  socket_ = WSASocket( AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0 );
  
  if( socket_ == INVALID_SOCKET )
  {
    throw( SockErr( WSAGetLastError(), "Failure to create socket." ) );
  }

  int eCode = bind( socket_, reinterpret_cast<sockaddr *>( &socketAddress_ ), sizeof( socketAddress_ ) );
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failure to bind socket." ) );
  }
}

bool TCPSocket::Connect( TCPSocket const &remoteSock )
{
  return Connect( remoteSock.socketAddress_ );
}

bool TCPSocket::Connect( SocketAddress const &remoteSock )
{
  bool connected = false;
  while( !connected )
  {
    sockaddr *pRemoteSockAddr = ( sockaddr * )( &remoteSock.adr_ );
    u32 addrSize = sizeof( remoteSock.adr_ );
    int eCode = connect( socket_, pRemoteSockAddr, addrSize );
    
    if( eCode == SOCKET_ERROR )
    {
      eCode = WSAGetLastError();

	  if( eCode == WSAEWOULDBLOCK )
		Sleep( 100 );  
      else 
	  if( eCode == WSAEALREADY )
        continue;
      else if( eCode == WSAEINVAL )
        continue;
      else if( eCode == WSAEISCONN )
        return true;

      throw( SockErr( eCode, "Failure to connect sockets." ) );
    }
  }

  return false;
}

void TCPSocket::Close( void )
{
  int eCode = closesocket( socket_ );

  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failure to close socket" ) );
  }
}

TCPSocket *TCPSocket::Accept( void )
{
  sockaddr_in tempAddress;
  SOCKET tempSocket;
  
  int addrSize = sizeof( sockaddr_in );
  sockaddr *endAddress = (sockaddr *) &tempAddress;


  tempSocket = accept( socket_, endAddress, &addrSize );
  if( tempSocket == INVALID_SOCKET )
  {
    int eCode = WSAGetLastError();
    
    if( eCode == WSAEWOULDBLOCK )
      return NULL;

    throw( SockErr( eCode, "Failure to accept connection." ) );
  }

  TCPSocket *endSocket = new TCPSocket();
  endSocket->socket_ = tempSocket;
  endSocket->socketAddress_ = tempAddress;
  return endSocket;
}

void TCPSocket::Listen( void )
{
  int eCode = listen( socket_, MAX_CONN_QUEUE );
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Socket failed to listen." ) );
  }
}

void TCPSocket::Receive( char const *initData, u32 dataSize )
{
  char buffer [ RECV_BUFFER_SIZE ];
  
  if( initData )
  {
    if( dataSize > RECV_BUFFER_SIZE )
    {
      printf( "Initial data was too large for receive buffer.\n" );
      return;
    }

    memcpy( buffer, initData, dataSize );
  }


  u32 offset;

  try
  {
    offset = ReceiveUntil( buffer, MsgHdr::GetSize(), RECV_BUFFER_SIZE, dataSize );
  }
  catch( SockErr e )
  {
    if( e.eCode_ == WSAEWOULDBLOCK )
      return;

    throw e;
  }

  MsgHdr header;
  header.ReadMessageHeader( buffer );

  memmove( buffer, buffer + MsgHdr::GetSize(), offset );

  offset = ReceiveUntil( buffer, header.msgSize_ - offset, RECV_BUFFER_SIZE, offset );
  
  DataBuffer data;
  data.Append( buffer, header.msgSize_ );

  inQueue_.Lock();
  MessageQueue &inQ( inQueue_.Access() );
  inQ.push_back( data );
  inQueue_.Unlock();

  if( offset > 0 )
  {
    memmove( buffer, buffer + header.msgSize_, offset );
    Receive( buffer, offset );
  }
}

bool TCPSocket::Receive( DataBuffer &data )
{
  char buffer [ RECV_BUFFER_SIZE ];
   
  data.Zero();
  int eCode;

  eCode = recv( socket_, buffer, sizeof( buffer ), 0 );

  if( eCode == 0 )
  {
    throw( SockErr( 0, "Remote end point has shutdown the connection." ) );
  }

  u32 totalBytesRead = 0;
  u32 bytesRead = 0;
  if( eCode == SOCKET_ERROR )
  {
    eCode = WSAGetLastError();
    if( eCode == WSAEWOULDBLOCK )
      return false;

    throw( SockErr( eCode, "Failure to receive." ) );
  }
  else
  {
    Sleep( 1 );
    totalBytesRead = eCode;
  }

  MsgHdr hdr;
  u32 hdrSize = hdr.ReadMessageHeader( buffer );
  bytesRead = totalBytesRead - hdrSize;
  data.Append( buffer + hdrSize, bytesRead );

  while( totalBytesRead < hdrSize + hdr.msgSize_ )
  {
    eCode = recv( socket_, buffer, sizeof( buffer ), 0 );
  
    if( eCode == 0 )
      disconnect_ = true;

    if( eCode == SOCKET_ERROR )
    {
      eCode = WSAGetLastError();
      if( eCode == WSAEWOULDBLOCK )
        continue;

      throw( SockErr( WSAGetLastError(), "Failure to receive." ) );
    }

    bytesRead = eCode;
    totalBytesRead += bytesRead;

    data.Append( buffer, bytesRead );
  }

  return true;
}


void TCPSocket::Send( DataBuffer const &data )
{
  u32 dataSize = data.Size();
  char sendBuffer [ SEND_BUFFER_SIZE ];
  MsgHdr hdr;
  hdr.msgSize_ = dataSize;

  u32 headerSize =  hdr.WriteMessageHeader( sendBuffer );
  char *dataStart = sendBuffer + headerSize;

  u32 bytesToSend = 0;
  if( SEND_BUFFER_SIZE < data.Size() + headerSize )
    bytesToSend = SEND_BUFFER_SIZE - headerSize;
  else
    bytesToSend = dataSize;

  memcpy( dataStart, data.Bytes(), bytesToSend );

  int eCode = send( socket_, sendBuffer, bytesToSend + headerSize, 0 );
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Client failed to send on local socket" ) );
  }

  u32 bytesSent = bytesToSend;

  while( bytesSent < dataSize )
  {
    if( dataSize - bytesSent < SEND_BUFFER_SIZE )
      bytesToSend = dataSize - bytesSent;
    else
      bytesToSend = SEND_BUFFER_SIZE;

    memcpy( sendBuffer, data.Bytes( bytesSent ), bytesToSend );
    eCode = send( socket_, sendBuffer, bytesToSend, 0 );
    if( eCode == SOCKET_ERROR )
    {
      throw( SockErr( WSAGetLastError(), "Client failed to send on local socket" ) );
    }

    bytesSent += bytesToSend;
  }
}

void TCPSocket::Shutdown( void )
{
  Sleep( 100 );
  int eCode = shutdown( socket_, SD_BOTH );

  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failed to shutdown on sending socket." ) );
  }
}

/*
void TCPSocket::ReceiveUntil( u32 recvCount )
{
  if( recvBuffer_.Size() >= recvCount )
    return;

  u32 writeCount = recvBuffer_.Slack();
  if( recvCount > writeCount )
    // to do:  throw something
    return 0;

  u32 totalBytesRead = recvBuffer_.Size();
  u32 bytesRead = 0;
  
  while( totalBytesRead < recvCount )
  {
    int eCode = recv( socket_, recvBuffer_.End(), recvBuffer_.Slack(), 0 );

    if( eCode == 0 )
    {
      disconnect_ = true;
      throw( SockErr( 0, "Remote end point has shutdown the connection." ) );
    }

    if( eCode == SOCKET_ERROR )
    {
      eCode = WSAGetLastError();


      throw( SockErr( WSAGetLastError(), "Failure to receive." ) );
    }

    bytesRead = eCode;
    totalBytesRead += bytesRead;
    buffer += bytesRead;
    writeCount -= bytesRead;
  }

  if( totalBytesRead > recvCount )
    return totalBytesRead - recvCount;
  else if( totalBytesRead < recvCount )
    // to do: this is bad, throw something
    return 0;
  else
    return 0;
}
*/

u32 TCPSocket::ReceiveUntil( char *buffer, u32 recvCount, u32 bufferSize, u32 bufferOffset /*= 0 */ )
{
  u32 writeCount = bufferSize - bufferOffset;
  if( recvCount > writeCount )
    // to do: throw something
    return 0;

  buffer += bufferOffset;
  u32 totalBytesRead = 0;
  u32 bytesRead = 0;

  while( totalBytesRead < recvCount )
  {
    int eCode = recv( socket_, buffer, writeCount, 0 );
  
    if( eCode == 0 )
      disconnect_ = true;

    if( eCode == 0 )
    {
      throw( SockErr( 0, "Remote end point has shutdown the connection." ) );
    }

    if( eCode == SOCKET_ERROR )
    {
      eCode = WSAGetLastError();

      /*
      if( eCode == WSAEWOULDBLOCK )
        continue;
      */

      throw( SockErr( WSAGetLastError(), "Failure to receive." ) );
    }

    bytesRead = eCode;
    totalBytesRead += bytesRead;
    buffer += bytesRead;
    writeCount -= bytesRead;
  }

  if( totalBytesRead > recvCount )
    return totalBytesRead - recvCount;
  else if( totalBytesRead < recvCount )
    // to do: this is bad, throw something
    return 0;
  else
    return 0;
}