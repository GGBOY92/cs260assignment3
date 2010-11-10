/*!
 *   @File   GameClient.hpp
 *   @Author Steven Liss
 *   @Date   16 Oct 2010
 *   @Brief  Client side game logic
 */

#pragma once

#include "Game.hpp"

class NetworkClient;
struct NetworkMessage;
class IOObject;

class GameClient
{
  NetworkClient *pNetCl_;
  IOObject *pConIn_;
  bool run_;
  bool myMove_;
  Game::Game myGame_;
  
  std::string userName_;
  std::string curOpponent_;
  
  Game::PlayersMessage plrsMsg_;

public:
  void Init( void );
  void Run( void );
  void Close( void );
  void PrintHelp( void );
  void ProcMessage( NetworkMessage const &nMsg );
  void ProcInput( std::string const &str );
  void PrintPlayers();
  void SendMoveMessage( std::string const & str );
  void SendPlayMessage( std::string const & str );
  void SendQuitMessage( void );
};
