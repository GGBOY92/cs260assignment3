
#pragma once

#include "SocketInterface.hpp"

#include <list>
#include <deque>

class UDPSocket : public iSocket
{

protected: // classes

  struct UDPMessageHeader
  {
    UDPMessageHeader()
    {
      msgSize_ = 0;
    }

    union
    {
      char cData_ [ 4 ];
      u32 msgSize_;
    };

    static u32 GetSize( void ) { return 4; }

    void WriteMessageHeader( char *buffer );
    void ReadMessageHeader( char const *buffer );
  };

  typedef UDPMessageHeader MsgHdr;

  typedef std::list< SocketAddress > AddressList;
  typedef std::deque< NetworkMessage > MessageQueue;

public: // methods

  UDPSocket();
  UDPSocket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );

  virtual bool Receive( NetworkMessage &rMessage );
  virtual void Send( NetworkMessage const &message );
  void SendTo( DataBuffer const &data, SocketAddress const &address );
  void Resend( void );

  void AcceptFrom( SocketAddress const & address );

private: // methods

  void Send( DataBuffer const &data );
  bool Receive( DataBuffer &data );

  bool ValidSender( SocketAddress const &address );

private: // members

  // a list of senders that this socket can receive from
  AddressList senders;

  // a list of sent messages that have not yet been acked
  MessageQueue sentQueue;
  
  u32 const static UDP_PACKET_SIZE = 10400;
};
