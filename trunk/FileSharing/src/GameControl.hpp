/*!
 *   @File   GameControl.hpp
 *   @Author Steven Liss
 *   @Date   16 Oct 2010
 *   @Brief  Client side network functionality
 */

#pragma once

#include "Game.hpp"
#include <vector>

class Server;
struct NetworkMessage;

class GameControl
{
  typedef std::vector< Game::Game > GameList;
  typedef std::vector< Game::ClientData > ClientList;
  ClientList clients_;
  GameList games_;

  Server *pServer_;
  bool running_;
  u32 gameID_;

  void ProcMessage( NetworkMessage const &netMsg );
  void SendPlayersMessage();
  void SendGameUpdate( Game::Game const &game );

  GameList::iterator GetGame( u32 gameID, bool &rFound );
  ClientList::iterator GetClientByName( char const *name_, bool &rFound );
  ClientList::iterator GetClientByConID( u32 conID, bool &rFound );

public:
  void Init( void );
  void Run( void );
  void Close( void );
  void CloseGame( GameList::iterator game );
  void SendPrintMessage( int sockID_, char const *msg );
  void SendGameEnd( int conID );
};
