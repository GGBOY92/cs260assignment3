/*!
 *   @File   GameClient.cpp
 *   @Author Steven Liss
 *   @Date   16 Oct 2010
 *   @Brief  Client side game logic
 */

#include "GameClient.hpp"
#include "NetworkClient.hpp"
#include "IOThread.hpp"
#include "SocketLibrary.hpp"

void GameClient::Init( void )
{
  pNetCl_ = new NetworkClient();
  pNetCl_->Init();
//  pConIn_ = new IOObject();
  myMove_ = false;
  run_ = true;
}

void GameClient::Run( void )
{
  using namespace Game;
  
  try
  {
    pNetCl_->Connect();
  }
  catch( BaseErrExcep e )
  {
    e.Print();
    return;
  }

  NetMsg nMsg;

  static bool ackUser = false;
  while( !ackUser )
  {
    Prompt( userName_, "Enter a username." );
    JoinMessage joinMsg;
    strcpy_s( joinMsg.data.userName_, MAX_USER_NAME_LEN + 1, userName_.c_str() );

    joinMsg >> nMsg;

    pNetCl_->SendMessage( nMsg );


    static bool resp = false;
    while( !resp )
    {
      if( pNetCl_->CheckForMessage( nMsg ) )
  	  {
	    resp = true;
		if( nMsg.type_ == NetMsg::ERR )
          printf( "The username selected is already in use or invalid\n" );
		else
		{
		  ackUser = true;
		  ProcMessage( nMsg );
		}
	  }
    }
	
	resp = false;
  }

  PrintHelp();
  IOObject::console.Wake();
  IOObject::console.Prompt();

  static bool inProc = false;
  while( pNetCl_->IsCon() && run_ )
  {
    if( inProc )
	{
	  inProc = false;
      IOObject::console.Prompt();
	}

    if( pNetCl_->CheckForMessage( nMsg ) )
	{
      ProcMessage( nMsg );
	}
    
    std::string str;
    if( IOObject::console.GetMessages( str ) )
    {
      ProcInput( str );
	  inProc = true;
//    IOObject::console.Resume();
    }
  }

  if( !pNetCl_->IsCon() )
    printf( "You may have lost connection with the server\n" );

  IOObject::console.Kill();
}

void GameClient::Close( void )
{
  pNetCl_->Close();
  delete pNetCl_;
}

void GameClient::PrintHelp( void )
{
  IOObject::console.StartPrint();
  printf( "At any time use this console with the following commands\n" );
  printf( "  Players      - see the list of connected players\n" );
  printf( "  Exit         - quit network chess including an active game\n" );
  printf( "  Play 'user'  - play chess with player specified by 'user'\n" );
  printf( "  Quit         - quit any active chess game\n" );
  printf( "  Move 'X@ Y#' - move the piece in row X col @ to row Y col # \n" );
  printf( "  Board        - view the game baord\n" );
  printf( "  Help         - see this help text at any time\n" );
  IOObject::console.EndPrint();
}

void GameClient::ProcMessage( NetworkMessage const &nMsg )
{
  using namespace Game;

  switch( nMsg.type_ )
  {
  case PLAYERS:
    plrsMsg_ << nMsg;
    break;
  case PLAY:
  {
    PlayMessage plyMsg;
    plyMsg << nMsg;
    curOpponent_ = plyMsg.data.userName_;
    myGame_.SetID( plyMsg.data.gameID_ );
	IOObject::console.Print( "%s is ready to play\n", curOpponent_.c_str() );
    break;
  }
  case UPDATE:
  {
    SyncGame updateMsg;
    updateMsg << nMsg;

    memcpy( myGame_.board, updateMsg.data_.board, sizeof( myGame_.board ) );
    
    myGame_.SetID( updateMsg.data_.gameID );
    myGame_.turn_ = updateMsg.data_.turn;
    myGame_.PrintBoard();

    break;
  }
  case PRINT:
  {
    PrintMessage pMsg;
    pMsg << nMsg;

    char msgBuf[ PrintMessage::MAX_MSG_SIZE + 1 ];
    memcpy( msgBuf, pMsg.data_.msg_, PrintMessage::MAX_MSG_SIZE );
    msgBuf[ PrintMessage::MAX_MSG_SIZE ] = 0;
    IOObject::console.Print( msgBuf );
    break;
  }
  case END:
  {
    curOpponent_.append( " has quit or been disconnected" );
    IOObject::console.Print( curOpponent_.c_str() );
    curOpponent_ = std::string();
    myGame_ = ChessGame();
  }
  default:
    break;
  }
}

void GameClient::ProcInput( std::string const &str )
{
  if( str == "Exit" )
  {
    if( myGame_.GetID() != 0 )
      SendQuitMessage();
    
    run_ = false;
  }
  if( str == "Help" )
    PrintHelp();
  if( str == "Quit" )
    SendQuitMessage();
  else if( str == "Players" )
    PrintPlayers();
  else if( str.compare( 0, 4, "Move", 0, 4 ) == 0 )
  {
    if( myGame_.GetID() == 0 )
      printf( "You should notice you're not currently playing chess.\n" );
    else
      SendMoveMessage( str );
  }
  else if( str.compare( 0, 4, "Play", 0, 4 ) == 0 )
  {
    if( myGame_.GetID() != 0 )
      printf( "You are already playing a game\n" );
    else
      SendPlayMessage( str );
  }
  else if( str == "Board" )
  {
    if( myGame_.GetID() == 0 )
		IOObject::console.Print( "You should notice you're not currently playing chess.\n" );
	else
	  myGame_.PrintBoard();
  }
  else
  {
//    if( gameID_ == 0 )
//      SendPlayMessage(  );
  }
}

void GameClient::PrintPlayers( void )
{
  plrsMsg_.Print();
}

void GameClient::SendQuitMessage( void )
{
  if( myGame_.GetID() == 0 )
    IOObject::console.Print( "You are not currently playing a game.\n" );
  else
  {
    Game::QuitGame qMsg;
    qMsg.data_.gameID_ = myGame_.GetID();
   
    NetworkMessage nMsg;
    qMsg >> nMsg;
    pNetCl_->SendMessage( nMsg );
    myGame_ = Game::Game();
  }
}

void GameClient::SendMoveMessage( std::string const & str )
{
  if( !myGame_.turn_ )
  {
	  IOObject::console.Print( "It is not your turn!\n" );
    return;
  }

  Game::MoveMessage msg;

  char rFrom;
  char rTo;
  u32  cFrom;
  u32  cTo;

  sscanf( str.c_str(), "Move %c%d %c%d", &rFrom, &cFrom, &rTo, &cTo );

  msg.data.fromRow_ = toupper( rFrom ) - 'A';
  msg.data.fromCol_ = cFrom - 1;

  msg.data.toRow_ = toupper( rTo ) - 'A';
  msg.data.toCol_ = cTo - 1;

  if( !myGame_.IsValidMove( msg.data.fromRow_, msg.data.fromCol_, msg.data.toRow_, msg.data.toCol_ ) )
  {
    IOObject::console.Print( "That move is not valid try another\n" );
    return;
  }

  msg.data.gameID_ = myGame_.GetID();

  NetMsg nMsg;
  msg >> nMsg;
  pNetCl_->SendMessage( nMsg );
}

void GameClient::SendPlayMessage( std::string const &str )
{
  myMove_ = false;
  Game::PlayMessage playMsg;
  strcpy_s( playMsg.data.userName_, Game::MAX_USER_NAME_LEN + 1, str.c_str() + 5 );

  if( strcmp( userName_.c_str(), playMsg.data.userName_ ) == 0 )
    printf( "You cannot play with yourself\n" );
  else
  {
    NetworkMessage nMsg;
    playMsg >> nMsg;
    pNetCl_->SendMessage( nMsg );
  }
}
