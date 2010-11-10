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

void TCPSocket::ReadMessageHeader( DataBuffer &data, char *buffer, u32 size )
{
  MsgHdr header;
  ReadUntil( data, buffer, header.GetSize() )
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

  while( totalBytesRead < hdrSize + hdr.size_ )
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
  hdr.size_ = dataSize;

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
