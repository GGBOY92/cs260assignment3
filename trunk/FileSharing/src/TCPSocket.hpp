/*!
 *   @File   Socket.hpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  A collection of routines and data for controlling socket operations
 */

#pragma once

#include "SocketInterface.hpp"
#include "SecureObject.hpp"
#include <string>
#include <deque>

class TCPSocket : public iSocket
{
private:
  typedef std::deque< DataBuffer > MessageQueue;

protected: // classes

    struct TCPMessageHeader
    {
        TCPMessageHeader()
        {
            msgSize_ = 0;
        }

        union
        {
            char cSize_ [4];
            u32 msgSize_;
        };

        static u32 GetSize( void ) { return 4; }

        u32 WriteMessageHeader( char *buffer );
        u32 ReadMessageHeader( char *buffer );
    };

    typedef TCPMessageHeader MsgHdr;

public: // methods

  TCPSocket();
  TCPSocket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );

  void Listen( void );
  bool Accept( TCPSocket &rSocket );
  bool Connect( TCPSocket const &remoteSock );
  bool Connect( SocketAddress const &remoteSock );

  // data functions
  virtual bool Receive( DataBuffer &data );
  virtual bool Receive( NetworkMessage &rMessage );
  void Receive( char const *initData = NULL, u32 dataSize = 0 );

  virtual void Send( DataBuffer const &data );
  virtual void Send( NetworkMessage const &message );
  
private: // members

  SecureObject< MessageQueue > inQueue_;
  
  u32 const static MAX_CONN_QUEUE = 10;
};
