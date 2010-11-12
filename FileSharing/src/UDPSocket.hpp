
#pragma once

#include "SocketInterface.hpp"

#include <list>

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

public: // methods

  UDPSocket();
  UDPSocket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );

  virtual bool Receive( DataBuffer &data );
  virtual void Send( DataBuffer const &data );
  void SendTo( DataBuffer const &data, SocketAddress const &address );
  
  virtual void Shutdown( void );
  virtual void Close( void );

  void ExpectFrom( SocketAddress const & address );

private: // members

  bool ValidSender( SocketAddress const &address );

private: // members

  // a list of senders that this socket can receive from
  AddressList senders;
  
  u32 const static UDP_PACKET_SIZE = 1400;
};
