#pragman once

#include "SocketInterface.hpp"

class UDPSocket : public iSocket
{
  UDPSocket();
  UDPSocket( std::string const &name );

  virtual void Init( void );
  virtual void InitBlocking( void );

  virtual bool Receive( DataBuffer &data );
  virtual void Send( DataBuffer const &data );
  
  virtual void Shutdown( void );
  virtual void Close( void );
};
