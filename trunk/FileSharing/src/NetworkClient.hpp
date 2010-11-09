/*!
 *   @File   NetworkClient.hpp
 *   @Author Steven Liss
 *   @Date   16 Oct 2010
 *   @Brief  Client side network functionality
 */

#pragma once

#include <string>

#include "Socket.hpp"

class NetworkClient
{
  Socket local_;
  SocketAddress remote_;
  bool connected_;

public:
  
  NetworkClient();
  
  void Init( void );
  void Connect( void );
  void Update( void );
  void Close( void );

  void SendMessage( NetworkMessage const &msg );
  void WaitForMessage( NetworkMessage &rMsg );
  bool CheckForMessage( NetMsg &nMsg );

  bool IsCon( void ) { return connected_; }
};
