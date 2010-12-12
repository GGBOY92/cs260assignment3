// ---------------------------------------------------------------------------
// Project Name		:	Asteroid Game
// File Name		:	GameState_Play.cpp
// Author			:	Sun Tjen Fam and Steven Liss
// Creation Date	:	2008/01/31
// Purpose			:	implementation of the 'play' game state
// History			:
// - 2008/01/31		:	- initial implementation
// ---------------------------------------------------------------------------

#include "mainclient.h"
#include <cmath>

#include "spaceships_defines.h"

//Game object structure
struct GameObj
{
	unsigned long		type;		// object type
	AEGfxTriList*		pMesh;		// This will hold the triangles which will form the shape of the object
};

// ---------------------------------------------------------------------------

//Game object instance structure
struct GameObjInst
{
	int					generation; // the generation of the object
	GameObj*			pObject;	// pointer to the 'original' shape
	unsigned long		flag;		// bit flag or-ed together
	float				scale;		// scaling value of the object instance
	AEVec2				posCurr;	// object current position
	AEVec2				velCurr;	// object current velocity
	float				dirCurr;	// object current direction

	AEMtx33				transform;	// object transformation matrix: Each frame, calculate the object instance's transformation matrix and save it here
};

// ---------------------------------------------------------------------------
// Static variables

// list of original object
static GameObj				sGameObjList[GAME_OBJ_NUM_MAX];				// Each element in this array represents a unique game object (shape)
static unsigned long		sGameObjNum;								// The number of defined game objects

// list of object instances
static GameObjInst			sGameObjInstList[GAME_OBJ_INST_NUM_MAX];	// Each element in this array represents a unique game object instance (sprite)
static unsigned long		sGameObjInstNum;							// The number of used game object instances

// pointer ot the ship object
static GameObjInst*			spShip;										// Pointer to the "Ship" game object instance

// number of ship available (lives 0 = game over)
static long					sShipLives;									// The number of lives left

// the score = number of asteroid destroyed
static unsigned long		sScore;										// Current score

// ---------------------------------------------------------------------------

// functions to create/destroy a game object instance
static GameObjInst*			gameObjInstCreate (unsigned long type, float scale, AEVec2* pPos, AEVec2* pVel, float dir, int gen);
static void					gameObjInstDestroy(GameObjInst* pInst);

// ---------------------------------------------------------------------------

// "Load" function of this state
void GameStateAsteroidsLoad(void)
{
	// zero the game object array
	memset(sGameObjList, 0, sizeof(GameObj) * GAME_OBJ_NUM_MAX);
	// No game objects (shapes) at this point
	sGameObjNum = 0;

	// zero the game object instance array
	memset(sGameObjInstList, 0, sizeof(GameObjInst) * GAME_OBJ_INST_NUM_MAX);
	// No game object instances (sprites) at this point
	sGameObjInstNum = 0;

	// The ship object instance hasn't been created yet, so this "spShip" pointer is initialized to 0
	spShip = 0;

	// load/create the mesh data (game objects / Shapes)
	GameObj* pObj;

	// =====================
	// create the ship shape
	// =====================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5f,  0.5f, 0x01FF0000, 
		-0.5f, -0.5f, 0xFFFF0000,
		 0.5f,  0.0f, 0xFFFFFFFF); 

	pObj->pMesh = AEGfxTriEnd();
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");


	// =======================
	// create the bullet shape
	// =======================

	GameObj* bObj;
	bObj = sGameObjList + sGameObjNum++;
	bObj->type = TYPE_BULLET;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5, 0.5, 0xFFFFFFFF,
		-0.5, -0.5, 0xFFFFFFFF,
		0.5, 0.5, 0xFFFFFFFF
		);
	AEGfxTriAdd(
		-0.5, -0.5, 0xFFFFFFFF,
		0.5, -0.5, 0xFFFFFFFF,
		0.5, 0.5, 0xFFFFFFFF
		);

	bObj->pMesh = AEGfxTriEnd();

	// =========================
	// create the asteroid shape
	// =========================

	
	GameObj* aObj;
	aObj = sGameObjList + sGameObjNum++;
	aObj->type = TYPE_ASTEROID;

	AEGfxTriStart();
	AEGfxTriAdd(
		-0.5, 0.5, 0xFFFFFFFF,
		-0.5, -0.5, 0xFFFFFFFF,
		0.5, 0.5, 0xFFFFFFFF
		);
	AEGfxTriAdd(
		-0.5, -0.5, 0xFFFFFFFF,
		0.5, -0.5, 0xFFFFFFFF,
		0.5, 0.5, 0xFFFFFFFF
		);

	aObj->pMesh = AEGfxTriEnd();
}

// ---------------------------------------------------------------------------

// "Initialize" function of this state
void GameStateAsteroidsInit(void)
{
	// create the main ship
	spShip = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, 0, 0, 0.0f, 0);
	AE_ASSERT(spShip);


	// CREATE THE INITIAL ASTEROIDS INSATNCES USING THE "GAMEOBJINSTCREATE" FUNCTION
	
    Vec2 initPos = Vec2(20, 20);
	Vec2 initVel = Vec2(5.0, 5.0);

	gameObjInstCreate(TYPE_ASTEROID, 10.0f, (AEVec2 *) &initPos, (AEVec2 *) &initVel, 0.0f, 0);

	Vec2Set(&initPos, -20.0f, -20.0f);
	Vec2Set(&initVel, 15.0f, 5.0f);
	

	gameObjInstCreate(TYPE_ASTEROID, 10.0f, (AEVec2 *) &initPos, (AEVec2 *) &initVel, 0.0f, 0);

	Vec2Set(&initPos, -20.0f, 40.0f);
	Vec2Set(&initVel, 2.0f, 15.0f);

	gameObjInstCreate(TYPE_ASTEROID, 10.0f, (AEVec2 *) &initPos, (AEVec2 *) &initVel, 0.0f, 0);
	

	// reset the score and the number of ship
	sScore      = 0;
	sShipLives    = SHIP_INITIAL_NUM;
}

// ---------------------------------------------------------------------------

// "Update" function of this state
void GameStateAsteroidsUpdate(void)
{
	// =========================
	// update according to input
	// =========================

	// This input handling moves the ship without any velocity nor acceleration
	// It should be changed when implementing the Asteroids project
	if (AEInputCheckCurr(DIK_UP))
	{
		sGameObjInstList->velCurr.x = .975 * (sGameObjInstList->velCurr.x + SHIP_ACCEL_FORWARD * gAEFrameTime * cosf(spShip->dirCurr) );
		sGameObjInstList->velCurr.y = .975 * (sGameObjInstList->velCurr.y + SHIP_ACCEL_FORWARD * gAEFrameTime * sinf(spShip->dirCurr) );
	}

	if (AEInputCheckCurr(DIK_DOWN))
	{
		sGameObjInstList->velCurr.x = .975 * (sGameObjInstList->velCurr.x + SHIP_ACCEL_BACKWARD * gAEFrameTime * cosf(spShip->dirCurr) );
		sGameObjInstList->velCurr.y = .975 * (sGameObjInstList->velCurr.y + SHIP_ACCEL_BACKWARD * gAEFrameTime * sinf(spShip->dirCurr) );
	}

	if (AEInputCheckCurr(DIK_LEFT))
	{
		spShip->dirCurr += SHIP_ROT_SPEED * (float)(gAEFrameTime);
		spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
	}

	if (AEInputCheckCurr(DIK_RIGHT))
	{
		spShip->dirCurr -= SHIP_ROT_SPEED * (float)(gAEFrameTime);
		spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
	}

	// Shoot a bullet if space is triggered (Create a new object instance)
	if (AEInputCheckTriggered(DIK_SPACE))
	{
		Vec2 bullPos = Vec2( spShip->posCurr.x + cosf(spShip->dirCurr), spShip->posCurr.y + sinf(spShip->dirCurr) );
		Vec2 bullVel = Vec2( cosf(spShip->dirCurr) * BULLET_SPEED, sinf(spShip->dirCurr) * BULLET_SPEED );
		gameObjInstCreate(TYPE_BULLET, 1.0f, (AEVec2 *) &bullPos, (AEVec2 *) &bullVel, 0.0f, 0);
	}

	// ==================================================
	// update physics of all active game object instances
	//	-- Positions are updated here
	// ==================================================
	
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		pInst->posCurr.x += pInst->velCurr.x * (float) gAEFrameTime;
		pInst->posCurr.y += pInst->velCurr.y * (float) gAEFrameTime;
	}

	// ===================================
	// update active game object instances
	// Example:
	//		-- Wrap specific object instances around the world (Needed for the assignment)
	//		-- Removing the bullets as they go out of bounds (Needed for the assignment)
	//		-- If you have a homing missile for example, compute its new orientation (Homing missiles are not required for the Asteroids project)
	//		-- Update a particle effect (Not required for the Asteroids project)
	// ===================================
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// check if the object is a ship
		if (pInst->pObject->type == TYPE_SHIP)
		{
			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - SHIP_SIZE, gAEWinMaxX + SHIP_SIZE);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - SHIP_SIZE, gAEWinMaxY + SHIP_SIZE);
		}

		// Wrap asteroids here
		if(pInst->pObject->type == TYPE_ASTEROID)
		{
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - ROID_SIZE, gAEWinMaxX + ROID_SIZE);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - ROID_SIZE, gAEWinMaxY + ROID_SIZE);
		}

		// Remove bullets that go out of bounds
		if(pInst->pObject->type == TYPE_BULLET)
		{
			if(pInst->posCurr.x < gAEWinMinX || pInst->posCurr.x > gAEWinMaxX || pInst->posCurr.y < gAEWinMinY || pInst->posCurr.y > gAEWinMaxY)
				gameObjInstDestroy(pInst);
		}
	}

	// ====================
	// check for collision
	// ====================
	
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* current = sGameObjInstList + i;

		// skip non-active object
		if ((current->flag & FLAG_ACTIVE) == 0)
			continue;

		if(current->pObject->type == TYPE_ASTEROID)
		{
			for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
			{
				GameObjInst* instCompare = sGameObjInstList + i;

				// skip non-active object
				if ((instCompare->flag & FLAG_ACTIVE) == 0)
					continue;
		
				if(instCompare->pObject->type == TYPE_ASTEROID)
					continue;

				if(instCompare->pObject->type == TYPE_SHIP)
				{
					if(TestRectToRect((Vec2 *) &(current->posCurr), current->scale / 2, current->scale / 2, (Vec2 *) &(instCompare->posCurr), 1.2f, 1.2f))
					{
						if(sShipLives <= 0)
						{
							instCompare->flag = 0;
							--sShipLives;
						}
						else
						{
							--sShipLives;
							spShip->posCurr.x = 0;
							spShip->posCurr.y = 0;
						}
					}
				}
				else if(instCompare->pObject->type == TYPE_BULLET)
				{
					if(TestPointToRect((Vec2 *) &(instCompare->posCurr), (Vec2 *) &(current->posCurr), current->scale, current->scale))
					{
						if(current->generation >= MAX_GENERATION)
						{
							gameObjInstDestroy(current);
							gameObjInstDestroy(instCompare);
							sScore += SCORE_ROID_DESTROYED;
						}
                        else
						{
							float newMass = sqrt(current->scale * current->scale / 2);
							Vec2 newPos = Vec2(current->posCurr.x - 5, current->posCurr.y);
							Vec2 newVel =  Vec2(instCompare->velCurr.x, instCompare->velCurr.y);
							Vec2RotVec(&newVel, -15);
							Vec2Normalize(&newVel, &newVel);
							Vec2Scale(&newVel, &newVel, Vec2Length((Vec2 *) &(current->velCurr)) );

							gameObjInstCreate(current->pObject->type, newMass, (AEVec2 *) &newPos, (AEVec2 *) &newVel, 0, ++current->generation);

			
							newPos = Vec2(current->posCurr.x + 5, current->posCurr.y);
							newVel = Vec2(instCompare->velCurr.x, instCompare->velCurr.y);
							Vec2RotVec(&newVel, 15);
							Vec2Normalize(&newVel, &newVel);
							Vec2Scale(&newVel, &newVel, Vec2Length((Vec2 *) &(current->velCurr)) );

							gameObjInstCreate(current->pObject->type, newMass, (AEVec2 *) &newPos, (AEVec2 *) &newVel, 0, ++current->generation);

							gameObjInstDestroy(instCompare);
							gameObjInstDestroy(current);
							sScore += SCORE_ROID_DESTROYED;
						}
					}
				}
			}
		}
	}


	// =====================================
	// calculate the matrix for all objects
	// =====================================

	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		Mtx33		 trans, rot, scale;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		Mtx33Scale(&scale, pInst->scale, pInst->scale);
		Mtx33RotRad(&rot, pInst->dirCurr);
		Mtx33Trans(&trans, pInst->posCurr.x, pInst->posCurr.y);

		Mtx33Concat((Mtx33 *) &(pInst->transform), &trans, &rot);
		Mtx33Concat((Mtx33 *) &(pInst->transform), (Mtx33 *) &(pInst->transform), &scale);

		// Compute the scaling matrix
		// Compute the rotation matrix 
		// Compute the translation matrix
		// Concatenate the 3 matrix in the correct order in the object instance's "transform" matrix

	}
}

// ---------------------------------------------------------------------------

void GameStateAsteroidsDraw(void)
{
	char strBuffer[1024];

	// draw all object instances in the list
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		//Set the current object instance's transform matrix using "AEGfxSetTransform"
		AEGfxSetTransform(&(pInst->transform));
		// Draw the shape used by the current object instance using "AEGfxTriDraw"
		AEGfxTriDraw(pInst->pObject->pMesh);

		//AEGfxPrint();
	}

	sprintf(strBuffer, "Score: %d", sScore);
	AEGfxPrint(10, 10, -1, strBuffer);

	sprintf(strBuffer, "Ship Left: %d", sShipLives >= 0 ? sShipLives : 0);
	AEGfxPrint(600, 10, -1, strBuffer);

	// display the game over message
	if (sShipLives < 0)
		AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");
}

// ---------------------------------------------------------------------------

void GameStateAsteroidsFree(void)
{
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* current = sGameObjInstList + i;

			gameObjInstDestroy(current);
	}
}

// ---------------------------------------------------------------------------

void GameStateAsteroidsUnload(void)
{
	for (unsigned long i = 0; i < sGameObjNum; i++)
	{
		GameObj* current = sGameObjList + i;

		AEGfxTriFree(current->pMesh);
	}
}

// ---------------------------------------------------------------------------

GameObjInst* gameObjInstCreate(unsigned long type, float scale, AEVec2* pPos, AEVec2* pVel, float dir, int gen)
{
	AEVec2 zero = { 0.0f, 0.0f };

	AE_ASSERT_PARM(type < sGameObjNum);
	
	// loop through the object instance list to find a non-used object instance
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// check if current instance is not used
		if (pInst->flag == 0)
		{
			// it is not used => use it to create the new instance
			pInst->generation = gen;
			pInst->pObject	= sGameObjList + type;
			pInst->flag		= FLAG_ACTIVE;
			pInst->scale	= scale;
			pInst->posCurr	= pPos ? *pPos : zero;
			pInst->velCurr	= pVel ? *pVel : zero;
			pInst->dirCurr	= dir;
			
			// return the newly created instance
			return pInst;
		}
	}

	// cannot find empty slot => return 0
	return 0;
}

// ---------------------------------------------------------------------------

void gameObjInstDestroy(GameObjInst* pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
}
// ---------------------------------------------------------------------------