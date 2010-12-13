// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Result.cpp
// Author			:	Sun Tjen Fam
// Creation Date	:	2008/02/05
// Purpose			:	implementation of the 'result' state
// History			:
// - 2008/02/05		:	- initial implementation
// ---------------------------------------------------------------------------

#include <algorithm>
#include <string>

#include "main.h"

std::vector<ResultStatus> results;

const unsigned ROW_HEIGHT = 30;
const unsigned COL_WIDTH = 230;

// ---------------------------------------------------------------------------
// Static variables

// ---------------------------------------------------------------------------
// function implementations

struct ResultSorter
{
    bool operator()(const ResultStatus& lhs, const ResultStatus& rhs) const
    {
        if(lhs.score_ > rhs.score_)
            return true;

        return false;
    }
};

void GameStateResultLoad(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameStateResultInit(void)
{ 
     std::sort(results.begin(), results.end(), ResultSorter());
}

// ---------------------------------------------------------------------------

void GameStateResultUpdate(void)
{
	if ((gAEFrameCounter > 60) && (AEInputCheckTriggered(DIK_SPACE)))
		gGameStateNext = GS_PLAY;
}

// ---------------------------------------------------------------------------

void GameStateResultDraw(void)
{
    AEGfxPrint(280, 20, 0xFFFFFFFF, "       RESULTS       ");

    unsigned xPos = 130;
    unsigned yPos = 130;


	  if (gAEFrameCounter > 30)
    {
        AEGfxPrint(130, 90, 0xFFFFFFFF, "User");
        AEGfxPrint(130, 110, 0xFFFFFFFF, "=============================================");
        AEGfxPrint(130 + COL_WIDTH, 90, 0xFFFFFFFF, "Score");

        char scoreBuffer[1000] = { 0 };
        char* pScore;

        bool first = true;
        for(std::vector<ResultStatus>::iterator it = results.begin(); it != results.end(); ++it)
        {
            AEGfxPrint(xPos, yPos, 0xFFFFFFFF, it->name_.name_);
            pScore = itoa(it->score_, scoreBuffer, 10);
            AEGfxPrint(xPos + COL_WIDTH, yPos, 0xFFFFFFFF, pScore);

            if(first)
            {
                first = false;
                AEGfxPrint(xPos + (COL_WIDTH * 2), yPos, 0xFF99FF00, "WINNER");
            }
            else
                AEGfxPrint(xPos + (COL_WIDTH * 2), yPos, 0xFFEE0000, "LOSER");


            yPos += ROW_HEIGHT;
        }
    }

    if(gAEFrameCounter > 60)
        AEGfxPrint(190, 560, 0xFFFFFFFF, "PRESS SPACE TO START NEXT ROUND");
}

// ---------------------------------------------------------------------------

void GameStateResultFree(void)
{
	// nothing
}

// ---------------------------------------------------------------------------

void GameStateResultUnload(void)
{
	// nothing
}

// ---------------------------------------------------------------------------
// Static function implementations

// ---------------------------------------------------------------------------










