// ---------------------------------------------------------------------------
// Project Name		:	Omega Library Test Program
// File Name		:	main.h
// Author			:	Sun Tjen Fam
// Creation Date	:	2007/04/26
// Purpose			:	main entry point for the test program
// History			:
// - 2007/04/26		:	- initial implementation
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// includes

#include "main.h"

#if (USE_CUSTOM_GAME_STATE_MGR == 0)
// include game state functions
#include "GameState_Play.h"
#include "GameState_Menu.h"
#include "GameState_Result.h"
#endif

// ---------------------------------------------------------------------------
// Static function protoypes

#if (USE_CUSTOM_GAME_STATE_MGR == 0)
static void addGameState();
#endif

// ---------------------------------------------------------------------------
// main

extern int gWIN_WIDTH;
extern int gWIN_HEIGHT;

int WINAPI WinMain(HINSTANCE instanceH, HINSTANCE prevInstanceH, LPSTR command_line, int show)
{
	gWIN_WIDTH = 800;
	gWIN_HEIGHT = 600;
	// Initialize the system
	AESysInit (instanceH, show);

#if (USE_CUSTOM_GAME_STATE_MGR == 0)
	// add the states to the game state list
	addGameState();
#endif

#if CLIENT_APP

	GameStateMgrInit(GS_MENU);

#else

	GameStateMgrInit(GS_PLAY);

#endif

	while(gGameStateCurr != GS_QUIT)
	{
		// reset the system modules
		AESysReset();

		// If not restarting, load the gamestate
		if(gGameStateCurr != GS_RESTART)
		{
			GameStateMgrUpdate();
			GameStateLoad();
		}
		else
			gGameStateNext = gGameStateCurr = gGameStatePrev;

		// Initialize the gamestate
		GameStateInit();

		while(gGameStateCurr == gGameStateNext)
		{
			AESysFrameStart();

			AEInputUpdate();

    		GameStateUpdate();

			GameStateDraw();

			AESysFrameEnd();

			// check if forcing the application to quit
			if ((gAESysWinExists == false) || AEInputCheckTriggered(DIK_ESCAPE))
				gGameStateNext = GS_QUIT;
		}
		
		GameStateFree();

		if(gGameStateNext != GS_RESTART)
			GameStateUnload();

		gGameStatePrev = gGameStateCurr;
		gGameStateCurr = gGameStateNext;
	}

	// free the system
	AESysExit();
}

// ---------------------------------------------------------------------------
// Static functions implementation

#if (USE_CUSTOM_GAME_STATE_MGR == 0)
void addGameState()
{
	AEGameStateMgrAdd(
		GS_PLAY, 
		GameStatePlayLoad,
		GameStatePlayInit,
		GameStatePlayUpdate,
		GameStatePlayDraw,
		GameStatePlayFree,
		GameStatePlayUnload);

	AEGameStateMgrAdd(
		GS_MENU, 
		GameStateMenuLoad,
		GameStateMenuInit,
		GameStateMenuUpdate,
		GameStateMenuDraw,
		GameStateMenuFree,
		GameStateMenuUnload);

	AEGameStateMgrAdd(
		GS_RESULT, 
		GameStateResultLoad,
		GameStateResultInit,
		GameStateResultUpdate,
		GameStateResultDraw,
		GameStateResultFree,
		GameStateResultUnload);
}
#endif

// ---------------------------------------------------------------------------


