/*!
 *   @File   shared.hpp
 *   @Author Steven Liss
 *   @Date   20 Sept 2010
 *   @Brief  A collection of routines and data shared by both the client and server application.
 */

#pragma once

#include <cstdio>
#include <string>

#include "CTypes.hpp"

struct NetworkMessage;

u32 const MAX_CONNECTIONS = 10;
typedef u32 MsgType;

struct Config
{
    void LoadConfigFile(void);

    std::string username_;

    u32 serverPort_;
    std::string serverIp_;
};

void WaitForInput( void );
bool PromptYesNo( char const *msg );
void Prompt( std::string &inBuffer, char const *msg );

struct BaseErrExcep
{
  BaseErrExcep( char const *msg ) : msg_( msg ) {}
  BaseErrExcep( std::string const &msg ) : msg_( msg ) {}
  
  void Print( void ) { printf("%s\n", msg_.c_str() ); }

  std::string msg_;
};

class DataBuffer
{

public:
  DataBuffer();
  DataBuffer( char const *bytes, u32 len );

  u32 Assign( char const *bytes, u32 count );
  u32 Append( char const *bytes, u32 count );
  void Allign( u32 pos );

  void Zero( void );
  u32 Size( void ) const { return len_; }
  void SetSize( u32 len ) { len_ = len; }
  u32 Slack( void ) const { return MAX_BUFFER_LEN - len_; }

  char *Bytes( void ) { return bytes_; }
  char *Bytes( u32 offset ) { return bytes_ + offset; }
  char *End( void ) { return bytes_ + len_; }

  char const *Bytes( void ) const { return bytes_; }
  char const *Bytes( u32 offset ) const { return bytes_ + offset; }
  void operator <<( NetworkMessage const &netMsg );

public: // members

  u32 const static MAX_BUFFER_LEN = 1800;

private: // members
  
  char bytes_[ MAX_BUFFER_LEN ];
  u32 len_;

};

template< typename T >
bool BetweenInc( T val, T lim1, T lim2 )
{
  if( val >= lim1 && val <= lim2 )
    return true;
  if( val >= lim2 && val <= lim1 )
    return true;

  return false;
}

