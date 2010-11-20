/*!
 *   @File   Server.hpp
 *   @Author Steven Liss
 *   @Date   14 Oct 2010
 *   @Brief  Server class for managing multiple connections through the socket handler class
 */

#pragma once

#include "TCPSocket.hpp"

#include <string>
#include <deque>
#include <vector>

class TCPSocketHandler;

class Server
{
private: // classes

  typedef std::vector< TCPSocketHandler * > ConnectionList;

private:

  /*
   *  Wait for a new client to connect.  This function blocks until a new client connects
   */
  void WaitForNewClient( TCPSocket &rSocket );
  
  /*
   *  Check for a new client trying to connect.  This function does not block.
   */
  bool CheckForNewClient( TCPSocket &rSocket );
  
  /*
   *  Create a new handler for the socket pointed by sock
   */
  void AddNewHandler( TCPSocket &sock );

  /*
   *  Collect all the inbound messages currently held by all handlers
   */
  void PullAllMessages( void );

  /*
   *  Send all the pending outbound messages on their appropriate connections
   */
  void PushAllMessages( void );

  /*
   *  Any handler that has been disconnected or generated an error it cannot recover from will be
   *  marked for death.  This function removes any of those dead handlers from the client connections
   */
  void RemoveDeadHandlers( void );

  /*
   *  This function
   */
  NetworkMessage PopInQueue();

  typedef std::deque< NetworkMessage > MessageQueue;
  
  // The inQueue contains all the inbound messages that have been passed up from the socket handlers
  MessageQueue inQueue_;
   
  // The outQueue contains all the outbound messages the server has yet to send
  MessageQueue outQueue_;

public:

  Server() { connections_.reserve( MAX_CONNECTIONS ); }

  /*
   *  find a socket handler by it's server assigned connection ID
   */
  TCPSocketHandler *GetHandler( u32 conID );

  /*
   *  start up winsock, load the server ip/port data, and initialize the listener socket
   */
  void Init( void );

  /*
   *  This function essentially calls update until it encounters an error.  The resultant behavior
   *  is that the server will constantly pull all the inbound messages and push out all the outbound
   *  messages.  This function is used when the server is running in it's own thread.
   */
  void Run( void );

  /*
   *  Multi responsibility function
   *
   *  1. Checks for new clients
   *  2. Pulls all inbound messages into the inQueue
   *  3. Pushes all outbound messages to the socket handlers
   *  4. Cleans up any dead handlers
   */
  void Update( void );

  /*
   *  shutdown the listener socket and close winsock
   */
  void Close( void );

  /*
   *  access to the listener socket.
   */
  TCPSocket &GetListener( void ) { return listener_; }

  /*
   *  fill the deque messages with all the current messages
   *
   *  WARNING: use this function at your own risk it is as of yet untested.
   */
  void GetMessages( std::deque< NetworkMessage > &messages );
  
  /*
   *  put the topmost message in the inQueue into msg, if the queue is empty return false otherwise
   *  return true
   */
  bool GetMessage( NetworkMessage &msg );

  /*
   *  Send out the network message msg.  Make sure msg has a valid connection ID.  If the socket cannot
   *  be found the message is discarded.
   */
  void SendMessage( NetworkMessage const &msg );
  void KillAllHandlers( void );

private: // members

  /*
   *  data structure holding all the servers client socket handlers
   */
  ConnectionList connections_;

  /*
   *  a counter used for assigning a unique connection ID
   */
  u32 conCount_;

  /*
   *  a TCP socket used to listen for incoming connections
   */
  TCPSocket listener_;

};
