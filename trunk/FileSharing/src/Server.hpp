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
  TCPSocket listener_;

  TCPSocket *WaitForNewClient( void );
  TCPSocket *CheckForNewClient( void );
  
  void AddNewHandler( TCPSocket *sock );
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

  TCPSocket &GetListener( void ) { return listener_; }
  void GetMessages( std::deque< NetworkMessage > &messages );
  
  bool GetMessage( NetworkMessage &msg );
  void SendMessage( NetworkMessage const &msg );
};
