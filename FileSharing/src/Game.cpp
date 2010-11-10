/*!
 *   @File   Game.cpp
 *   @Author Steven Liss
 *   @Date   22 Oct 2010
 *   @Brief  Common game data, messages, net functionality, etc.
 */

#include "Game.hpp"
#include "SocketLibrary.hpp"
#include "IOThread.hpp"

namespace Game
{
  void JoinMessage::operator<<( NetworkMessage const &netMsg )
  {
    memcpy( bytes_, netMsg.msg_.Bytes(), sizeof( bytes_ ) );
  }

  void JoinMessage::operator>>( NetworkMessage &netMsg )
  {
    netMsg.type_ = JOIN;
    netMsg.msg_.Assign( bytes_, sizeof( bytes_ ) );
  }

  void MoveMessage::operator<<( NetworkMessage const &netMsg )
  {
    memcpy( bytes_, netMsg.msg_.Bytes(), sizeof( bytes_ ) );
  }

  void MoveMessage::operator>>( NetworkMessage &netMsg )
  {
    netMsg.type_ = MOVE;
    netMsg.msg_.Assign( bytes_, sizeof( bytes_ ) );
  }

  void PlayersMessage::operator<<( NetworkMessage const &netMsg )
  {
    memcpy( Bytes(), netMsg.msg_.Bytes(), sizeof( Data ) );
  }

  void PlayersMessage::operator>>( NetworkMessage &netMsg )
  {
    netMsg.type_ = PLAYERS;
    netMsg.msg_.Assign( Bytes(), sizeof( Data ) );
  }

  void PlayersMessage::Print( void )
  {
    for( u32 i = 0; i < data_.numUsers_; ++i )
    {
      ClientData &user = data_.users_[ i ];
      if( user.sockID_ != UNUSED )
      {
        if( user.gameID_ == 0 )
          printf( "%s, available\n", user.userName_ );
        else
          printf( "%s, in game\n", user.userName_ );
      }
    }
  }

  void PlayMessage::operator<<( NetworkMessage const &netMsg )
  {
    memcpy( bytes_, netMsg.msg_.Bytes(), sizeof( bytes_ ) );
  }

  void PlayMessage::operator>>( NetworkMessage &netMsg )
  {
    netMsg.type_ = PLAY;
    netMsg.msg_.Assign( bytes_, sizeof( bytes_ ) );
  }

  void SyncGame::operator <<( NetworkMessage const &netMsg )
  {
    memcpy( bytes_, netMsg.msg_.Bytes(), sizeof( bytes_ ) );
  }

  void SyncGame::operator>>( NetworkMessage &netMsg )
  {
    netMsg.type_ = UPDATE;
    netMsg.msg_.Assign( bytes_, sizeof( bytes_ ) );
  }

  void QuitGame::operator <<( NetworkMessage const &netMsg )
  {
    memcpy( bytes_, netMsg.msg_.Bytes(), sizeof( bytes_ ) );
  }

  void QuitGame::operator>>( NetworkMessage &netMsg )
  {
    netMsg.type_ = QUIT;
    netMsg.msg_.Assign( bytes_, sizeof( bytes_ ) );
  }

  void PrintMessage::operator <<( NetworkMessage const &netMsg )
  {
    memcpy( bytes_, netMsg.msg_.Bytes(), sizeof( bytes_ ) );
  }

  void PrintMessage::operator>>( NetworkMessage &netMsg )
  {
    netMsg.type_ = PRINT;
    netMsg.msg_.Assign( bytes_, sizeof( bytes_ ) );
  }

  void Game::PrintBoard( void )
  {
    IOObject::console.StartPrint();

    printf( "\n" );
    printf( "  12345678\n" );
    printf( "\n" );
    for( u32 i = 0; i < Game::BOARD_DIM; ++i )
    {
      printf( "%c %s \n", 'A' + i, board[ i ] );
    }

	if( turn_ )
      IOObject::console.Print( "Make your move" );
	else
      IOObject::console.Print( "Waiting for opponent" );

	IOObject::console.EndPrint();
  }
}
