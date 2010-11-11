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

public:
  TCPSocket();
  TCPSocket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );

  void Listen( void );
  TCPSocket *Accept( void );
  bool Connect( TCPSocket const &remoteSock );
  bool Connect( SocketAddress const &remoteSock );

  // data functions
  virtual bool Receive( DataBuffer &data );
  void Receive( char const *initData = NULL, u32 dataSize = 0 );
  virtual void Send( DataBuffer const &data );
  
  // clean up functions
  virtual void Shutdown( void );
  virtual void Close( void );
  u32 TCPSocket:: ReceiveUntil( char *buffer, u32 recvCount, u32 bufferSize, u32 bufferOffset = 0 );

private: // members

  SecureObject< MessageQueue > inQueue_;
};
