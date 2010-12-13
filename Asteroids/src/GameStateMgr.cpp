// ---------------------------------------------------------------------------
// Project Name		:	Asteroid
// File Name		:	GameStateMgr.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the game state manager
// History			:
// - 2008/02/08		:	- modified to be used like the old style game state
//						  manager
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "GameStateMgr.h"

#include "GameState_Play.h"
#include "GameState_Single.h"
#include "GameState_Menu.h"
#include "GameState_Result.h"
#include "GameState_Platform.h"

// ---------------------------------------------------------------------------
// globals

// variables to keep track the current, previous and next game state
u32	gGameStateInit;
u32	gGameStateCurr;
u32	gGameStatePrev;
u32	gGameStateNext;

// pointer to functions for game state life cycles functions
void (*GameStateLoad)()		= 0;
void (*GameStateInit)()		= 0;
void (*GameStateUpdate)()	= 0;
void (*GameStateDraw)()		= 0;
void (*GameStateFree)()		= 0;
void (*GameStateUnload)()	= 0;

// ---------------------------------------------------------------------------
// Functions implementations

void GameStateMgrInit(u32 gameStateInit)
{
	// set the initial game state
	gGameStateInit = gameStateInit;

	// reset the current, previoud and next game
	gGameStateCurr = 
	gGameStatePrev = 
	gGameStateNext = gGameStateInit;

	// call the update to set the function pointers
	GameStateMgrUpdate();
}

// ---------------------------------------------------------------------------

void GameStateMgrUpdate()
{
	if ((gGameStateCurr == GS_RESTART) || (gGameStateCurr == GS_QUIT))
		return;

	switch (gGameStateCurr)
	{
	case GS_PLAY:
		GameStateLoad	= GameStatePlayLoad;
		GameStateInit	= GameStatePlayInit;
		GameStateUpdate	= GameStatePlayUpdate;
		GameStateDraw	= GameStatePlayDraw;
		GameStateFree	= GameStatePlayFree;
		GameStateUnload	= GameStatePlayUnload;
		break;

	case GS_SINGLE:
		GameStateLoad	= GameStateSingleLoad;
		GameStateInit	= GameStateSingleInit;
		GameStateUpdate	= GameStateSingleUpdate;
		GameStateDraw	= GameStateSingleDraw;
		GameStateFree	= GameStateSingleFree;
		GameStateUnload	= GameStateSingleUnload;
		break;


	case GS_MENU:
		GameStateLoad	= GameStateMenuLoad;
		GameStateInit	= GameStateMenuInit;
		GameStateUpdate	= GameStateMenuUpdate;
		GameStateDraw	= GameStateMenuDraw;
		GameStateFree	= GameStateMenuFree;
		GameStateUnload	= GameStateMenuUnload;
		break;

	case GS_RESULT:
		GameStateLoad	= GameStateResultLoad;
		GameStateInit	= GameStateResultInit;
		GameStateUpdate	= GameStateResultUpdate;
		GameStateDraw	= GameStateResultDraw;
		GameStateFree	= GameStateResultFree;
		GameStateUnload	= GameStateResultUnload;
		break;

	default:
		AE_FATAL_ERROR("invalid state!!");
	}
}

// ---------------------------------------------------------------------------
