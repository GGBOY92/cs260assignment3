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

void LoadConfigFile( u32 &port, char *ip = NULL );
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
  u32 const static MAX_BUFFER_LEN = 1000;
  char bytes_[ MAX_BUFFER_LEN ];
  u32 len_;

public:
  DataBuffer();
  DataBuffer( char const *bytes, u32 len );

  void Assign( char const *bytes, u32 count );
  void Append( char const *bytes, u32 count );
  void Zero( void );
  u32 Size( void ) const { return len_; }
  char const *Bytes( void ) const { return bytes_; }
  char const *Bytes( u32 offset ) const { return bytes_ + offset; }
  void operator <<( NetworkMessage const &netMsg );
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

