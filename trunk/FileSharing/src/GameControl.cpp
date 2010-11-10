/*!
 *   @File   GameControl.cpp
 *   @Author Steven Liss
 *   @Date   16 Oct 2010
 *   @Brief  Server side game logic
 */

#include "GameControl.hpp"
#include "Server.hpp"

void GameControl::Init()
{
  pServer_ = new Server();
  pServer_->Init();

  gameID_ = 1;
}

void GameControl::Run()
{
  running_ = true;
  while( running_ )
  {
    pServer_->Update();
    
    NetworkMessage msg;
    while( pServer_->GetMessage( msg ) )
      ProcMessage( msg );
  }
}

void GameControl::Close()
{
  pServer_->Close();
  delete pServer_;
}

void GameControl::ProcMessage( NetworkMessage const &netMsg )
{
  using namespace Game;

  switch( netMsg.type_ )
  {
    case JOIN:
    {
      JoinMessage jMsg;
      jMsg << netMsg;

	  bool cExists;
      ClientList::iterator clientIt2 = GetClientByName( jMsg.data.userName_, cExists );

	  if( cExists )
	  {
	    NetMsg nMsg;
		nMsg.type_ = NetMsg::ERR;
		nMsg.conID_ = netMsg.conID_;
		pServer_->SendMessage( nMsg );
		break;
	  }
      
      ClientData cData;
      strcpy_s( cData.userName_, MAX_USER_NAME_LEN + 1, jMsg.bytes_ );
      cData.sockID_ = netMsg.conID_;

      clients_.push_back( cData );

      SendPlayersMessage();

      break;
    }
  case MOVE:
  {
    MoveMessage mMsg;
    mMsg << netMsg;

    bool gExists;
    GameList::iterator game = GetGame( mMsg.data.gameID_, gExists );
    
    if( !gExists )
      return;

    game->Move( mMsg.data.fromRow_, mMsg.data.fromCol_, mMsg.data.toRow_, mMsg.data.toCol_ );
    game->turn_ = !game->turn_;

    SendGameUpdate( *game );

    break;
  }
  case PLAY:
  {
    PlayMessage playMsg;
    playMsg << netMsg;

    
    bool c1Exists;
    ClientList::iterator clientIt1 = GetClientByConID( netMsg.conID_, c1Exists );
    
    bool c2Exists;
    ClientList::iterator clientIt2 = GetClientByName( playMsg.data.userName_, c2Exists );

    if( !c1Exists || !c2Exists || clientIt2->gameID_ != 0 )
    {
      printf( "user unavailable" );
      //Send back no game message
    }
    else
    {
      ClientData &oppData1 = *clientIt1;
      ClientData &oppData2 = *clientIt2;
      
      ChessGame game( gameID_ );
      
      oppData1.gameID_ = game.GetID();
      oppData2.gameID_ = game.GetID();

      oppData1.opSockID_ = oppData2.sockID_;
      oppData2.opSockID_ = oppData1.sockID_;

      game.firstOppConID_ = oppData1.sockID_;
      game.secondOppConID_ = oppData2.sockID_;

      strcpy_s( game.firstOppName_, MAX_USER_NAME_LEN + 1, oppData1.userName_ );
      strcpy_s( game.secondOppName_, MAX_USER_NAME_LEN + 1, oppData2.userName_ );

      games_.push_back( game );

      ++gameID_;

      PlayMessage playMsg;
      strcpy_s( playMsg.data.userName_, MAX_USER_NAME_LEN + 1, oppData1.userName_ );

      NetMsg nMsg;
      playMsg >> nMsg;
      nMsg.conID_ = oppData2.sockID_;
      pServer_->SendMessage( nMsg );
      
      strcpy_s( playMsg.data.userName_, MAX_USER_NAME_LEN + 1, oppData2.userName_ );
      playMsg >> nMsg;
      nMsg.conID_ = oppData1.sockID_;
      pServer_->SendMessage( nMsg );

      SendGameUpdate( game );
      SendPlayersMessage();
    }

    break;
  }
  case QUIT:
  {
    QuitGame qMsg;
    qMsg << netMsg;
   
    bool gExists;
    GameList::iterator game = GetGame( qMsg.data_.gameID_, gExists );

    if( !gExists )
      return;

    if( game->firstOppConID_ == netMsg.conID_ )
      SendGameEnd( game->secondOppConID_ );
    else
      SendGameEnd( game->firstOppConID_ );

    CloseGame( game );
    SendPlayersMessage();
    
    break;
  }
  case PRINT:
  {

    break;
  }
  case NetMsg::DISCON:
  {
    bool cExists;
    ClientList::iterator clientIt = GetClientByConID( netMsg.conID_, cExists );

    if( cExists )
    {
      if( clientIt->gameID_ != 0 )
      {
        bool gExists;
        GameList::iterator gameIt = GetGame( clientIt->gameID_, gExists );
        
        if( gameIt->firstOppConID_ == clientIt->sockID_ )
          SendGameEnd( gameIt->secondOppConID_ );
        else
          SendGameEnd( gameIt->firstOppConID_ );

        if( gExists )
        {
          CloseGame( gameIt );
        }
      }

      clients_.erase( clientIt );
      SendPlayersMessage();
    }

    break;
  }
  default:
    break;
  }
}

void GameControl::SendGameUpdate( Game::Game const &game )
{
  Game::SyncGame syn;
  memcpy( syn.data_.board, game.board, sizeof( game.board ) );
  syn.data_.gameID = game.GetID();
  syn.data_.turn = game.turn_;

  NetMsg nMsg;
  syn >> nMsg;
  nMsg.conID_ = game.firstOppConID_;
  pServer_->SendMessage( nMsg );

  syn.data_.turn = !game.turn_;

  syn >> nMsg;
  nMsg.conID_ = game.secondOppConID_;
  pServer_->SendMessage( nMsg );
}

void GameControl::SendPlayersMessage()
{
  Game::PlayersMessage plrsMsg;

  u32 numClients = plrsMsg.data_.numUsers_ = clients_.size();
  for( u32 i = 0; i < numClients; ++i )
  {
    memcpy( plrsMsg.Bytes(), &( clients_[ 0 ] ), sizeof( Game::ClientData ) * numClients );
    NetMsg netMsg;
    plrsMsg >> netMsg;
    netMsg.conID_ = clients_[ i ].sockID_;
    pServer_->SendMessage( netMsg );
  }
}

GameControl::ClientList::iterator GameControl::GetClientByName( char const *name_, bool &rFound )
{
  for( ClientList::iterator clientIt = clients_.begin(); clientIt != clients_.end(); ++clientIt )
  {
    Game::ClientData &data = *clientIt;
    if( strcmp( data.userName_, name_ ) == 0 )
    {
      rFound = true;
      return clientIt;
    }
  }

  printf( "Attempted to find a user that doesn't exist\n" );

  rFound = false;
  return clients_.end();
}

GameControl::ClientList::iterator GameControl::GetClientByConID( u32 conID, bool &rFound )
{
  for( ClientList::iterator clientIt = clients_.begin(); clientIt != clients_.end(); ++clientIt )
  {
    Game::ClientData &data = *clientIt;
    if( data.sockID_ == conID )
    {
      rFound = true;
      return clientIt;
    }
  }

  printf( "Attempted to find a user that doesn't exist\n" );

  rFound = false;
  return clients_.end();
}

GameControl::GameList::iterator GameControl::GetGame( u32 gameID, bool &rFound )
{
  for( std::vector< Game::Game >::iterator gameIt = games_.begin(); gameIt != games_.end(); ++gameIt )
  {
    Game::Game &curGame = *gameIt;
    if( curGame.GetID() == gameID )
    {
      rFound = true;
      return gameIt;
    }
  }

  printf( "Attempted to find a game that doesn't exist\n" );

  rFound = false;
  return games_.end();
}

void GameControl::CloseGame( GameList::iterator game )
{
  using namespace Game;

  bool c1Exists;
  ClientList::iterator clientIt1 = GetClientByConID( game->firstOppConID_, c1Exists );

  bool c2Exists;
  ClientList::iterator clientIt2 = GetClientByConID( game->secondOppConID_, c2Exists );
  
  if( c1Exists )
  {
    ClientData &oppData1 = *clientIt1;
    oppData1.gameID_ = 0;
    oppData1.opSockID_ = UNUSED;
  }

  if( c2Exists )
  {
    ClientData &oppData2 = *clientIt2;
    oppData2.gameID_ = 0;
    oppData2.opSockID_ = UNUSED;
  }

  games_.erase( game );
}

void GameControl::SendPrintMessage( int sockID_, char const *msg )
{
  Game::PrintMessage pMsg( msg );

  NetMsg nMsg;
  pMsg >> nMsg;
  nMsg.conID_ = sockID_;

  pServer_->SendMessage( nMsg );
}

void GameControl::SendGameEnd( int conID )
{
  NetMsg nMsg;
  nMsg.conID_ = conID;
  nMsg.type_ = Game::END;
  pServer_->SendMessage( nMsg );
}