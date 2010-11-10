#pragma once

#include "SocketLibrary.hpp"

class iSocket
{
public: // classes
  
  struct SocketErrExcep : public WSErr
  {
    SocketErrExcep( int eCode, char const *msg, std::string const &socketName = std::string() );
    SocketErrExcep( int eCode, std::string const &msg, std::string const &socketName = std::string() );
    void Print( void );

    // data
    std::string socketName_;
  };

  typedef SocketErrExcep SockErr;

private: // classes
public: // methods

  iSocket( void )
  {
    PrepareSocket();
  }

  iSocket( std::string const &name ) : name_( name )
  {
    PrepareSocket();
  }

  void SetIP( char *IP );
  void SetPortNumber( u32 port );

  virtual void Init( void ) = 0;
  virtual void InitBlocking( void ) = 0;
  virtual bool Connect( SocketAddress const &remoteSock ) = 0;
  virtual void Close( void ) = 0;

  virtual bool Accept( iSocket &endPoint ) = 0;
  virtual void Listen( void ) = 0;
  virtual bool Receive( DataBuffer &data ) = 0;
  virtual void Send( DataBuffer const &data ) = 0;
  virtual void Shutdown( void ) = 0;


private: // methods
  void PrepareSocket( void )
  {
    SecureZeroMemory( &socketAddress_, sizeof( socketAddress_ ) );
  }

public: // members
protected: // members

  sockaddr_in socketAddress_;
  SOCKET socket_;
  std::string name_;
  bool disconnect_;
  
  u32 const static MAX_CONN_QUEUE = 10;
  u32 const static SEND_BUFFER_SIZE = 1000;
  u32 const static RECV_BUFFER_SIZE = 1000;

private: // members

  friend class Socket;
};
