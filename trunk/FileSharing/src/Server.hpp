/*!
 *   @File   Server.hpp
 *   @Author Steven Liss
 *   @Date   14 Oct 2010
 *   @Brief  Server class for managing multiple connections
 */

#pragma once

#include "Socket.hpp"

#include <string>
#include <deque>
#include <vector>

class SocketHandler;

class Server
{
private:
  Socket listener_;

  Socket WaitForNewClient( void );
  bool CheckForNewClient( Socket &client );
  
  void AddNewHandler( Socket const &sock );
  typedef std::vector< SocketHandler * > ConnectionList;
  ConnectionList connections_;
  u32 conCount_;
  
  void PullAllMessages( void );
  void PushAllMessages( void );
  void RemoveDeadHandlers( void );
  NetworkMessage PopInQueue();

  typedef std::deque< NetworkMessage > MessageQueue;
  MessageQueue inQueue_;
  MessageQueue outQueue_;

public:
  Server() { connections_.reserve( MAX_CONNECTIONS ); }
  SocketHandler *GetHandler( u32 conID );

  void Init( void );
  void Run( void );
  void Update( void );
  void Close( void );

  Socket &GetListener( void ) { return listener_; }
  void GetMessages( std::deque< NetworkMessage > &messages );
  
  bool GetMessage( NetworkMessage &msg );
  void SendMessage( NetworkMessage const &msg );
};
