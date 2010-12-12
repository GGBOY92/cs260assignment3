
#pragma once

#include "SocketInterface.hpp"

#include <list>
#include <deque>

class UDPSocket : public iSocket
{

protected: // classes

  struct UDPMessageHeader
  {
    enum{
      // high level packet types
      MESSAGE = 0,

      // socket level packet types
      ACK,

      // always last in enum
      SIZE
    };
    UDPMessageHeader()
    {
      msgSize_ = 0;
      packetID_ = 0;
      sentCount_ = 0;
      headerType_ = MESSAGE;
    }

    union
    {
      struct
      {
        u32 msgSize_;
        u32 packetID_;
        u32 sentCount_;
        u32 headerType_;
      };
      char cData_ [ 16 ];
    };

    static u32 GetSize( void ) { return 16; }

    void WriteMessageHeader( char *buffer );
    void ReadMessageHeader( char const *buffer );
  };

  typedef UDPMessageHeader MsgHdr;

  typedef std::list< SocketAddress > AddressList;
  typedef std::deque< std::pair< UDPMessageHeader, NetworkMessage > > MessageQueue;

public: // methods

  UDPSocket();
  UDPSocket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );

  virtual bool Receive( NetworkMessage &rMessage );
  virtual void Send( NetworkMessage const &message );
  void SendTo( DataBuffer const &data, MsgHdr header, SocketAddress const &address );
  void Resend( void );

  void AcceptFrom( SocketAddress const & address );
  void AcceptAny( void );

private: // methods

  void ProcessAcknowledgement( MsgHdr const &header );
  void Acknowledge( MsgHdr const &header, SocketAddress ackRecipient );
  bool Receive( DataBuffer &data, MsgHdr &rHeader, SocketAddress &rSenderAddress );

  bool ValidSender( SocketAddress const &address );

private: // members

  // a list of senders that this socket can receive from
  AddressList senders;

  // a list of sent messages that have not yet been acked
  MessageQueue sentQueue;

  u32 m_sent_count;
  
  u32 m_wait_count;

  bool m_accept_any;
  
  u32 static currentID_;
  u32 const static UDP_PACKET_SIZE = 1800;
  u32 const static MAX_SEND_COUNT = 500;
  u32 const static MAX_WAIT = 30;
};
