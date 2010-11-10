/*!
 *   @File   Game.hpp
 *   @Author Steven Liss
 *   @Date   22 Oct 2010
 *   @Brief  Common game data, messages, net functionality, etc.
 */

#pragma once

#include "shared.hpp"

#include <vector>

struct NetworkMessage;

namespace Game
{
  int const UNUSED = -1;
  u32 const MAX_USER_NAME_LEN = 20;
  
  class Game
  {
    u32 const static BOARD_DIM = 8;
    u32 gameID_;

  public:
    bool turn_;
    u32 firstOppConID_;
    u32 secondOppConID_;

    char firstOppName_[ MAX_USER_NAME_LEN + 1 ];
    char secondOppName_[ MAX_USER_NAME_LEN + 1 ];
    
    typedef char Board[ BOARD_DIM ][ BOARD_DIM + 1 ];
    Board board;

    u32 GetID( void ) const { return gameID_; }
    void SetID( u32 gID ) { gameID_ = gID; }

	bool IsValidMove( u32 rFrom, u32 cFrom, u32 rTo, u32 cTo )
	{
      if( !BetweenInc< u32 >( rFrom, 0, BOARD_DIM - 1 ) )
	    return false;
      if( !BetweenInc< u32 >( cFrom, 0, BOARD_DIM - 1 ) )
	    return false;
      if( !BetweenInc< u32 >( rTo, 0, BOARD_DIM - 1 ) )
	    return false;
      if( !BetweenInc< u32 >( cTo, 0, BOARD_DIM - 1 ) )
	    return false;

	  if( board[ rFrom ][ cFrom ] == '#' )
        return false;

	  return true;
	}

    void Move( u32 rFrom, u32 cFrom, u32 rTo, u32 cTo )
    {
      board[ rTo ][ cTo ] = board[ rFrom ][ cFrom ];
      board[ rFrom ][ cFrom ] = '#';
    }

	void SetupBoard( void )
	{
	  //clear squares
      memset( board[ 0 ], '#', sizeof( Board ) );

	  //set both rows of pawns
	  memset( board[ 1 ], 'P', BOARD_DIM );
      memset( board[ BOARD_DIM - 2 ], 'p', BOARD_DIM );


      board[ 0 ][ 0 ] = board[ 0 ][ BOARD_DIM - 1 ] = 'R';
      board[ 0 ][ 1 ] = board[ 0 ][ BOARD_DIM - 2 ] = 'N';
      board[ 0 ][ 2 ] = board[ 0 ][ BOARD_DIM - 3 ] = 'B';
      board[ 0 ][ 3 ] = 'Q';
      board[ 0 ][ 4 ] = 'K';

      board[ BOARD_DIM - 1 ][ 0 ] = board[ BOARD_DIM - 1 ][ BOARD_DIM - 1 ] = 'r';
      board[ BOARD_DIM - 1 ][ 1 ] = board[ BOARD_DIM - 1 ][ BOARD_DIM - 2 ] = 'n';
      board[ BOARD_DIM - 1 ][ 2 ] = board[ BOARD_DIM - 1 ][ BOARD_DIM - 3 ] = 'b';
      board[ BOARD_DIM - 1 ][ 4 ] = 'q';
      board[ BOARD_DIM - 1 ][ 3 ] = 'k';
     

      for( u32 i = 0; i < BOARD_DIM; ++i )
        board[ i ][ BOARD_DIM ] = 0;
	}

    Game() : gameID_( 0 )
    {
	  SetupBoard();
	}
    
    Game( u32 gameID ) : gameID_( gameID ), turn_( false )
    {
      SetupBoard();
    }

    void PrintBoard( void );
  };

  typedef Game ChessGame;
  
  struct ClientData
  {
    ClientData() : 
      gameID_( 0 ), sockID_( UNUSED ), opSockID_( UNUSED )
    {}

    char userName_[ MAX_USER_NAME_LEN + 1 ];
    u32 gameID_;
    int sockID_;
    int opSockID_;
  };

  enum{
    JOIN = 0,
    MOVE,
    PLAYERS,
    PLAY,
    UPDATE,
    QUIT,
    PRINT,
    END
  };

  struct JoinMessage
  {
    struct Data
    {
      char userName_[ MAX_USER_NAME_LEN + 1 ];
    };

    union
    {
      Data data;
      char bytes_[ sizeof( Data ) ];
    };

    void operator<<( NetworkMessage const &netMsg );
    void operator>>( NetworkMessage &netMsg );
  };

  struct MoveMessage
  {
    struct Data
    {
      u32 fromCol_;
      u32 fromRow_;
      u32 toCol_;
      u32 toRow_;
      int gameID_;
    };

    union
    {
      Data data;
      char bytes_[ sizeof( Data ) ];
    };

    void operator<<( NetworkMessage const &netMsg );
    void operator>>( NetworkMessage &netMsg );
  };

  struct PlayersMessage
  {
    PlayersMessage() { data_.numUsers_ = 0; }
    struct Data
    {
      ClientData users_[ MAX_CONNECTIONS ];
      u32 numUsers_;
    };

    Data data_;
    char *Bytes( void ) { return (char *) &data_; } 


    void operator<<( NetworkMessage const &netMsg );
    void operator>>( NetworkMessage &netMsg );
    void Print( void );
  };

  struct PlayMessage
  {
    struct Data
    {
      char userName_[ MAX_USER_NAME_LEN + 1 ];
      u32 gameID_;
    };

    union
    {
      Data data;
      char bytes_[ sizeof( Data ) ];
    };

    void operator<<( NetworkMessage const &netMsg );
    void operator>>( NetworkMessage &netMsg );
  };

  struct SyncGame
  {
    struct Data
    {
      Game::Board board;
      u32 gameID;
      bool turn;
    };

    union
    {
      Data data_;
      char bytes_[ sizeof( Data ) ];
    };

    void operator<<( NetworkMessage const &netMsg );
    void operator>>( NetworkMessage &netMsg );
  };

  struct QuitGame
  {
    struct Data
    {
      u32 gameID_;
    };

    union
    {
      Data data_;
      char bytes_[ sizeof( Data ) ];
    };

    void operator<<( NetworkMessage const &netMsg );
    void operator>>( NetworkMessage &netMsg );
  };

  struct PrintMessage
  {
    u32 const static MAX_MSG_SIZE = 300;

    PrintMessage()
    {}

    PrintMessage( char const *str )
    {
      strcpy_s( bytes_, MAX_MSG_SIZE, str );
    }

    struct Data
    {
      char msg_[ MAX_MSG_SIZE ];
    };

    union{
      Data data_;
      char bytes_[ sizeof( Data ) ];
    };

    void operator<<( NetworkMessage const &netMsg );
    void operator>>( NetworkMessage &netMsg );
  };
}
