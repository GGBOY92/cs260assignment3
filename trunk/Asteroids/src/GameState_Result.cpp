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

const unsigned ROW_HEIGHT = 50;
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
//     std::string larry = "Larry";
//     std::string robert = "Robert";
//     std::string steven = "Steven";
// 
//     ResultStatus status;
//     strcpy(status.name_.name_, larry.c_str());
//     status.score_ = 10;
// 
//     results.push_back(status);
// 
//     strcpy(status.name_.name_, steven.c_str());
//     status.score_ = 99;
// 
//     results.push_back(status);
// 
//     strcpy(status.name_.name_, robert.c_str());
//     status.score_ = 70;
// 
//     results.push_back(status);
// 
     std::sort(results.begin(), results.end(), ResultSorter());
}

// ---------------------------------------------------------------------------

void GameStateResultUpdate(void)
{
	if ((gAEFrameCounter > 60) && (AEInputCheckTriggered(DIK_SPACE)))
		gGameStateNext = GS_MENU;
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

        AEGfxPrint(xPos + (COL_WIDTH * 2), yPos, 0xFF99FF00, "WINNER");
        for(std::vector<ResultStatus>::iterator it = results.begin(); it != results.end(); ++it)
        {
            AEGfxPrint(xPos, yPos, 0xFFFFFFFF, it->name_.name_);
            pScore = itoa(it->score_, scoreBuffer, 10);
            AEGfxPrint(xPos + COL_WIDTH, yPos, 0xFFFFFFFF, pScore);

            yPos += ROW_HEIGHT;
        }
    }

    if(gAEFrameCounter > 60)
        AEGfxPrint(190, 560, 0xFFFFFFFF, "PRESS SPACE TO RETURN TO MAIN MENU");
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










