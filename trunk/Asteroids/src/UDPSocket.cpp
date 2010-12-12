
#include "UDPSocket.hpp"

#include <algorithm>

u32 UDPSocket::currentID_ = 0;

UDPSocket::UDPSocket() : iSocket(), m_wait_count( 0 )
{}


UDPSocket::UDPSocket( std::string const &name ) : iSocket( name ), m_wait_count( 0 )
{}


void UDPSocket::Init( void )
{
  socket_ = WSASocket( AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, 0 );
  if( socket_ == INVALID_SOCKET )
  {
    throw( SockErr( WSAGetLastError(), "Failure to create UDP socket." ) );
  }


  ULONG flags = 1;
  int eCode = ioctlsocket( socket_, FIONBIO, &flags ); 
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failed to set the UDP socket non-blocking" ) );
  }


  eCode = bind( socket_, reinterpret_cast<sockaddr *>( &socketAddress_ ), sizeof( socketAddress_ ) );
  if( eCode == SOCKET_ERROR )
  {
    throw( SockErr( WSAGetLastError(), "Failure to bind UDP socket." ) );
  }
}


void UDPSocket::InitBlocking( void )
{
  // to do: implement
}


bool UDPSocket::Receive( NetworkMessage &rMessage )
{
  DataBuffer messageBuffer;
  MsgHdr header;
  SocketAddress senderAddress;

  bool status = Receive( messageBuffer, header, senderAddress );

  if( status )
  {
    rMessage << messageBuffer;
    rMessage.receiverAddress_ = senderAddress;
    Acknowledge( header, senderAddress );
  }

  return status;
}


bool UDPSocket::Receive( DataBuffer &data, MsgHdr &rHeader, SocketAddress &rSenderAddress )
{
  char buffer[ UDP_PACKET_SIZE ];

  int size = sizeof( rSenderAddress.adr_ );
  int eCode = recvfrom( socket_, buffer, UDP_PACKET_SIZE, 0, ( sockaddr * ) &rSenderAddress.adr_, &size );
  
  if( eCode == SOCKET_ERROR )
  {
    eCode = WSAGetLastError();

    if( eCode == WSAEWOULDBLOCK )
      return false;
    else if( eCode == WSAECONNRESET )
      return false;

    throw( SockErr( eCode, "Error in UDPSocket::Receive\n" ) );
  }

  if( !ValidSender( rSenderAddress ) )
    return false;

  rHeader.ReadMessageHeader( buffer );

  if( rHeader.headerType_ == MsgHdr::ACK )
  {
    ProcessAcknowledgement( rHeader );
    return Receive( data, rHeader, rSenderAddress );
  }

  data.Assign( buffer + MsgHdr::GetSize(), rHeader.msgSize_ );

  return true;
}


void UDPSocket::SendTo( DataBuffer const &data, MsgHdr header, SocketAddress const &address )
{
  if( data.Size() + MsgHdr::GetSize() > UDP_PACKET_SIZE )
    printf( "packet truncation in UDPSokcet::SendTo\n" );

  u32 bytesSent = 0;
  u32 totalBytesSent = 0;

  char buffer[ UDP_PACKET_SIZE ];

  header.WriteMessageHeader( buffer );
  memcpy( buffer + MsgHdr::GetSize(), data.Bytes(), data.Size() );

  while( totalBytesSent < UDP_PACKET_SIZE )
  {
    sockaddr *pSAddr = ( sockaddr * ) &address.adr_;
    int size = sizeof( address.adr_ );
    int eCode = sendto( socket_, buffer, UDP_PACKET_SIZE, 0, pSAddr, size );

    if( eCode == SOCKET_ERROR )
    {
      eCode = WSAGetLastError();
      if( eCode == WSAEWOULDBLOCK )
        continue;

      throw( SockErr( eCode, "SendTo generated an error\n" ) );
    }
    else
    {
      bytesSent = eCode;
    }

    totalBytesSent += bytesSent;
  }
}


void UDPSocket::Send( NetworkMessage const &message )
{
  DataBuffer messageBuffer;
  messageBuffer << message;
  SocketAddress receiverAddress = message.receiverAddress_;
  
  MsgHdr header;
  header.msgSize_ = messageBuffer.Size();
  header.packetID_ = currentID_++;
  header.headerType_ = MsgHdr::MESSAGE;

  SendTo( messageBuffer, header, receiverAddress );

  ++m_sent_count;
  
  // collect the message and store it for retransmission
  std::pair< MsgHdr, NetworkMessage > messagePair;
  messagePair.first = header;
  messagePair.second = message;
  sentQueue.push_back( messagePair );

}

void UDPSocket::AcceptFrom( SocketAddress const & address )
{
  senders.push_back( address );
}


bool UDPSocket::ValidSender( SocketAddress const &address )
{
  if( m_accept_any )
      return true;

  AddressList::iterator addIt = std::find( senders.begin(), senders.end(), address );

  if( addIt == senders.end() )
    return false;
  else
    return true;
}


void UDPSocket::Resend( void )
{
  /*
  if( m_sent_count >= 1 )
  {
    m_sent_count = 0;
    return;
  }
  */

  for( MessageQueue::iterator msgIt = sentQueue.begin(); msgIt != sentQueue.end(); )
  {
    ++m_wait_count;

    if( m_wait_count != MAX_WAIT )
        continue;
    else
        m_wait_count = 0;

    MsgHdr &header = msgIt->first;
    NetworkMessage &netMessage = msgIt->second;

    ++header.sentCount_;

    if( header.sentCount_ > MAX_SEND_COUNT )
      msgIt = sentQueue.erase( msgIt );
    else
    {
      DataBuffer messageBuffer;
      messageBuffer << netMessage;

      SendTo( messageBuffer, header, netMessage.receiverAddress_ );

      ++msgIt;
    }
  }
}


void UDPSocket::ProcessAcknowledgement( MsgHdr const &header )
{
  for( MessageQueue::iterator msgIt = sentQueue.begin(); msgIt != sentQueue.end(); ++msgIt )
  {
    MsgHdr curHeader = msgIt->first;
    if( curHeader.packetID_ == header.packetID_ )
      sentQueue.erase( msgIt );

    return;
  }
}


void UDPSocket::Acknowledge( MsgHdr const &header, SocketAddress ackRecipient )
{
  MsgHdr ackHeader;
  ackHeader.msgSize_ = 0;
  ackHeader.sentCount_ = 0;
  ackHeader.packetID_ = header.packetID_;
  ackHeader.headerType_ = MsgHdr::ACK;

  SendTo( DataBuffer(), ackHeader, ackRecipient );
}


void UDPSocket::AcceptAny( void )
{
    m_accept_any = true;
}


void UDPSocket::UDPMessageHeader::WriteMessageHeader( char *buffer )
{
  memcpy( buffer, cData_, GetSize() );
}


void UDPSocket::UDPMessageHeader::ReadMessageHeader( char const *buffer )
{
  memcpy( cData_, buffer, GetSize() );
}
