
struct UserInfo
{
    Username m_username;
    SocketAddress m_address;
    u32 m_id;
    GameObjInst *m_p_obj;

    bool operator ==( UserInfo const &rhs )
    {
        return ( m_username == rhs.m_username );
    }
};

typedef std::map< std::string, UserInfo * > UserTable;
UserTable users;

typedef std::vector< ResultStatus > ScoreList;
ScoreList scores;

struct InputInfo
{
    UserInfo *m_p_user;
    u32 m_key;
    KeyState m_key_state;
};

typedef std::vector<InputInfo> InputList;
InputList inputs;


UDPSocket *p_main_socket;
char *local_address;

f64 static init_time = 0.0;
f64 static live_time = 0.0;
bool static counting_down = false;

void ProcInput( void );
void ProcMessage( NetworkMessage &netMessage );
void SendToAll( NetworkMessage &net_msg );
void SendUpdate( void );
void SendScores( bool final = false );

// ---------------------------------------------------------------------------
//     SERVER CODE
// ---------------------------------------------------------------------------

void GameStatePlayLoad(void)
{
	// zero the game object list
	memset(sGameObjList, 0, sizeof(GameObj) * GAME_OBJ_NUM_MAX);
	sGameObjNum = 0;

	// zero the game object instance list
	memset(sGameObjInstList, 0, sizeof(GameObjInst) * GAME_OBJ_INST_NUM_MAX);
	sGameObjInstNum = 0;

	spShip = 0;

	// load/create the mesh data
	loadGameObjList();

	// initialize the initial number of asteroid
	sAstCtr = NULL;
}

// ---------------------------------------------------------------------------

void GameStatePlayInit(void)
{
    ///////////////////////
    // Socket stuff
    ///////////////////////
    try
    {
        StartWinSock();
    }
    catch(WSErr& e)
    {
        e.Print();
        exit(1);
    }

    Config config;
    config.LoadConfigFile();
    GetLocalIP( local_address );

    p_main_socket = new UDPSocket();
    p_main_socket->SetIP( local_address );
    p_main_socket->SetPortNumber( config.serverPort_ );

    try
    {
        p_main_socket->Init();   
    }
    catch( iSocket::SockErr& e )
    {
        CloseWinSock();
        e.Print();
        exit(1);
    }

    p_main_socket->AcceptAny();


    ///////////////////////
    // Other stuff
    ///////////////////////
    // reset the number of current asteroid and the total allowed
	sAstCtr = 0;
	sAstNum = AST_NUM_MIN;

	// create the main ship
//	spShip = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, 0, 0, 0.0f, true);
	
 //   AE_ASSERT(spShip);

#if !NO_ROIDS

	// get the time the asteroid is created
	sAstCreationTime = AEGetTime();
	
	// generate the initial asteroid
	for (u32 i = 0; i < sAstNum; i++)
		astCreate(0);

#endif

	// reset the score and the number of ship
	sScore      = 0;
	sShipCtr    = SHIP_INITIAL_NUM;
	sSpecialCtr = SHIP_SPECIAL_NUM;

	// reset the delay to switch to the result state after game over
	sGameStateChangeCtr = 2.0f;
}

// ---------------------------------------------------------------------------

void GameStatePlayUpdate(void)
{
    if( counting_down )
    {
        live_time = AEGetTime() - init_time;

        if( live_time > GAME_TIME_SEC )
        {
            SendScores( true );
            gGameStateNext = GS_RESULT;
            return;
        }
    }

    p_main_socket->Resend();

    NetworkMessage netMessage;

//    printf( "pending bytes are %u\n", p_main_socket->PendingBytes() );

    try
    {
        while( p_main_socket->Receive( netMessage ) )
            ProcMessage( netMessage );
    }
    catch( iSocket::SockErr err )
    {
        err.Print();
    }

    ProcInput();
    SendUpdate();

    static u32 i = 0;
    i++;

    if( i % 60 == 0 )
        SendScores();

	// ==================================
	// create new asteroids if necessary
	// ==================================

#if !NO_ROIDS

	if ((sAstCtr < sAstNum) && ((AEGetTime() - sAstCreationTime) > AST_CREATE_DELAY))
	{
		// keep track the last time an asteroid is created
		sAstCreationTime = AEGetTime();

		// create an asteroid
		astCreate(0);
	}

#endif

	// ===============
	// update physics
	// ===============

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		// update the position
		AEVec2ScaleAdd(&pInst->posCurr, &pInst->velCurr, &pInst->posCurr, (f32)(gAEFrameTime));
	}

	// ===============
	// update objects
	// ===============

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
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
		// check if the object is an asteroid
		else if (pInst->pObject->type == TYPE_ASTEROID)
		{
			AEVec2 u;
			f32    uLen;

			// warp the asteroid from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, gAEWinMinX - AST_SIZE_MAX, gAEWinMaxX + AST_SIZE_MAX);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, gAEWinMinY - AST_SIZE_MAX, gAEWinMaxY + AST_SIZE_MAX);

			// pull the asteroid toward the ship a little bit
			if (spShip)
			{
				// apply acceleration propotional to the distance from the asteroid to the ship
				AEVec2Sub	(&u, &spShip->posCurr, &pInst->posCurr);
				AEVec2Scale	(&u, &u, AST_TO_SHIP_ACC * (f32)(gAEFrameTime));
				AEVec2Add	(&pInst->velCurr, &pInst->velCurr, &u);
			}

			// if the asterid velocity is more than its maximum velocity, reduce its speed
			if ((uLen = AEVec2Length(&pInst->velCurr)) > (AST_VEL_MAX * 2.0f))
			{
				AEVec2Scale	(&u, &pInst->velCurr, (1.0f / uLen) * (AST_VEL_MAX * 2.0f - uLen) * pow(AST_VEL_DAMP, (f32)(gAEFrameTime)));
				AEVec2Add	(&pInst->velCurr, &pInst->velCurr, &u);
			}
		}
		// check if the object is a bullet
		else if (pInst->pObject->type == TYPE_BULLET)
		{
			// kill the bullet if it gets out of the screen
			if (!AEInRange(pInst->posCurr.x, gAEWinMinX - AST_SIZE_MAX, gAEWinMaxX + AST_SIZE_MAX) || 
				!AEInRange(pInst->posCurr.y, gAEWinMinY - AST_SIZE_MAX, gAEWinMaxY + AST_SIZE_MAX))
				gameObjInstDestroy(pInst);
		}
		// check if the object is a bomb
		else if (pInst->pObject->type == TYPE_BOMB)
		{
			// adjust the life counter
			pInst->life -= (f32)(gAEFrameTime) / BOMB_LIFE;
			
			if (pInst->life < 0.0f)
			{
				gameObjInstDestroy(pInst);
			}
			else
			{
				f32    radius = 1.0f - pInst->life;
				AEVec2 u;

				pInst->dirCurr += 2.0f * PI * (f32)(gAEFrameTime);

				radius =   1.0f - radius;
				radius *=  radius;
				radius *=  radius;
				radius *=  radius;
				radius *=  radius;
				radius =   (1.0f - radius) * BOMB_RADIUS;

				// generate the particle ring
				for (u32 j = 0; j < 10; j++)
				{
					//f32 dir = AERandFloat() * 2.0f * PI;
					f32 dir = (j / 9.0f) * 2.0f * PI + pInst->life * 1.5f * 2.0f * PI;

					u.x = AECos(dir) * radius + pInst->posCurr.x;
					u.y = AESin(dir) * radius + pInst->posCurr.y;

					//sparkCreate(PTCL_EXHAUST, &u, 1, dir + 0.8f * PI, dir + 0.9f * PI);
					sparkCreate(PTCL_EXHAUST, &u, 1, dir + 0.40f * PI, dir + 0.60f * PI);
				}
			}
		}
		// check if the object is a missile
		else if (pInst->pObject->type == TYPE_MISSILE)
		{
			// adjust the life counter
			pInst->life -= (f32)(gAEFrameTime) / MISSILE_LIFE;
			
			if (pInst->life < 0.0f)
			{
				gameObjInstDestroy(pInst);
			}
			else
			{
				AEVec2 dir;

				if (pInst->pUserData == 0)
				{
					pInst->pUserData = missileAcquireTarget(pInst);
				}
				else
				{
					GameObjInst* pTarget = (GameObjInst*)(pInst->pUserData);

					// if the target is no longer valid, reacquire
					if (((pTarget->flag & FLAG_ACTIVE) == 0) || (pTarget->pObject->type != TYPE_ASTEROID))
						pInst->pUserData = missileAcquireTarget(pInst);
				}

				if (pInst->pUserData)
				{
					GameObjInst* pTarget = (GameObjInst*)(pInst->pUserData);
					AEVec2 u;
					f32    uLen;

					// get the vector from the missile to the target and its length
					AEVec2Sub(&u, &pTarget->posCurr, &pInst->posCurr);
					uLen = AEVec2Length(&u);

					// if the missile is 'close' to target, do nothing
					if (uLen > 0.1f)
					{
						// normalize the vector from the missile to the target
						AEVec2Scale(&u, &u, 1.0f / uLen);

						// calculate the missile direction vector
						AEVec2Set(&dir, AECos(pInst->dirCurr), AESin(pInst->dirCurr));

						// calculate the cos and sin of the angle between the target 
						// vector and the missile direction vector
						f32 cosAngle = AEVec2DotProduct(&dir, &u), 
							sinAngle = AEVec2CrossProductMag(&dir, &u), 
							rotAngle;

						// calculate how much to rotate the missile
						if (cosAngle < AECos(MISSILE_TURN_SPEED * (f32)(gAEFrameTime)))
							rotAngle = MISSILE_TURN_SPEED * (f32)(gAEFrameTime);
						else
							rotAngle = AEACos(AEClamp(cosAngle, -1.0f, 1.0f));

						// rotate to the left if sine of the angle is positive and vice versa
						pInst->dirCurr += (sinAngle > 0.0f) ? rotAngle : -rotAngle;
					}
				}

				// adjust the missile velocity
				AEVec2Set  (&dir, AECos(pInst->dirCurr), AESin(pInst->dirCurr));
				AEVec2Scale(&dir, &dir, MISSILE_ACCEL * (f32)(gAEFrameTime));
				AEVec2Add  (&pInst->velCurr, &pInst->velCurr, &dir);
				AEVec2Scale(&pInst->velCurr, &pInst->velCurr, pow(MISSILE_DAMP, (f32)(gAEFrameTime)));

				sparkCreate(PTCL_EXHAUST, &pInst->posCurr, 1, pInst->dirCurr + 0.8f * PI, pInst->dirCurr + 1.2f * PI);
			}
		}
		// check if the object is a particle
		else if ((TYPE_PTCL_WHITE <= pInst->pObject->type) && (pInst->pObject->type <= TYPE_PTCL_RED))
		{
			pInst->scale   *= pow(PTCL_SCALE_DAMP, (f32)(gAEFrameTime));
			pInst->dirCurr += 0.1f;
			AEVec2Scale(&pInst->velCurr, &pInst->velCurr, pow(PTCL_VEL_DAMP, (f32)(gAEFrameTime)));

			if (pInst->scale < PTCL_SCALE_DAMP)
				gameObjInstDestroy(pInst);
		}
	}

	// ====================
	// check for collision
	// ====================

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pSrc = sGameObjInstList + i;

		// skip non-active object
		if ((pSrc->flag & FLAG_ACTIVE) == 0)
			continue;

		if ((pSrc->pObject->type == TYPE_BULLET) || (pSrc->pObject->type == TYPE_MISSILE))
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;

				// skip no-active and non-asteroid object
				if (((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				if (AETestPointToRect(&pSrc->posCurr, &pDst->posCurr, pDst->scale, pDst->scale) == false)
					continue;
				
				if (pDst->scale < AST_SIZE_MIN)
				{
					sparkCreate(PTCL_EXPLOSION_M, &pDst->posCurr, (u32)(pDst->scale * 10), pSrc->dirCurr - 0.05f * PI, pSrc->dirCurr + 0.05f * PI, pDst->scale);                   

                    if( pSrc->parent != NULL )
                    {
                        if( ( pSrc->parent->flag & FLAG_ACTIVE ) != 0 )
                        {
                            ( pSrc->parent->score ) += 10;
                        }
                    }


					if ((sScore % AST_SPECIAL_RATIO) == 0)
						sSpecialCtr++;
					if ((sScore % AST_SHIP_RATIO) == 0)
						sShipCtr++;
					if (sScore == sAstNum * 5)
						sAstNum = (sAstNum < AST_NUM_MAX) ? (sAstNum * 2) : sAstNum;

					// destroy the asteroid
					gameObjInstDestroy(pDst);
				}
				else
				{
                    if( pSrc->parent != NULL )
                    {
                        if( ( pSrc->parent->flag & FLAG_ACTIVE ) != 0 )
                        {
                            ++( pSrc->parent->score );
                        }
                    }

					sparkCreate(PTCL_EXPLOSION_S, &pSrc->posCurr, 10, pSrc->dirCurr + 0.9f * PI, pSrc->dirCurr + 1.1f * PI);

					// impart some of the bullet/missile velocity to the asteroid
					AEVec2Scale(&pSrc->velCurr, &pSrc->velCurr, 0.01f * (1.0f - pDst->scale / AST_SIZE_MAX));
					AEVec2Add  (&pDst->velCurr, &pDst->velCurr, &pSrc->velCurr);

					// split the asteroid to 4
					if ((pSrc->pObject->type == TYPE_MISSILE) || ((pDst->life -= 1.0f) < 0.0f))
						astCreate(pDst);
				}

				// destroy the bullet
				gameObjInstDestroy(pSrc);

				break;
			}
		}
		else if (TYPE_BOMB == pSrc->pObject->type)
		{
			f32 radius = 1.0f - pSrc->life;

			pSrc->dirCurr += 2.0f * PI * (f32)(gAEFrameTime);

			radius =   1.0f - radius;
			radius *=  radius;
			radius *=  radius;
			radius *=  radius;
			radius *=  radius;
			radius *=  radius;
			radius =   (1.0f - radius) * BOMB_RADIUS;

			// check collision
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;

				if (((pDst->flag & FLAG_ACTIVE) == 0) || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				if (AECalcDistPointToRect(&pSrc->posCurr, &pDst->posCurr, pDst->scale, pDst->scale) > radius)
					continue;

				if (pDst->scale < AST_SIZE_MIN)
				{
					f32 dir = atan2f(pDst->posCurr.y - pSrc->posCurr.y, pDst->posCurr.x - pSrc->posCurr.x);

					gameObjInstDestroy(pDst);
					sparkCreate(PTCL_EXPLOSION_M, &pDst->posCurr, 20, dir + 0.4f * PI, dir + 0.45f * PI);
					sScore++;

					if ((sScore % AST_SPECIAL_RATIO) == 0)
						sSpecialCtr++;
					if ((sScore % AST_SHIP_RATIO) == 0)
						sShipCtr++;
					if (sScore == sAstNum * 5)
						sAstNum = (sAstNum < AST_NUM_MAX) ? (sAstNum * 2) : sAstNum;
				}
				else
				{
					// split the asteroid to 4
					astCreate(pDst);
				}
			}
		}
		else if (pSrc->pObject->type == TYPE_ASTEROID)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;
				f32          d;
				AEVec2       nrm, u;

				// skip no-active and non-asteroid object
				if ((pSrc == pDst) || ((pDst->flag & FLAG_ACTIVE) == 0)  || (pDst->pObject->type != TYPE_ASTEROID))
					continue;

				// check if the object rectangle overlap
				d = AECalcDistRectToRect(
					&pSrc->posCurr, pSrc->scale, pSrc->scale, 
					&pDst->posCurr, pDst->scale, pDst->scale, 
					&nrm);
				
				if (d >= 0.0f)
					continue;
				
				// adjust object position so that they do not overlap
				AEVec2Scale	(&u, &nrm, d * 0.25f);
				AEVec2Sub	(&pSrc->posCurr, &pSrc->posCurr, &u);
				AEVec2Add	(&pDst->posCurr, &pDst->posCurr, &u);

				// calculate new object velocities
				resolveCollision(pSrc, pDst, &nrm);
			}
		}
		else if (pSrc->pObject->type == TYPE_SHIP)
		{
			for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
			{
				GameObjInst* pDst = sGameObjInstList + j;

				// skip no-active and non-asteroid object
				if ( ( pSrc == pDst )
                    || ( ( pDst->flag & FLAG_ACTIVE) == 0 )
                    || ( pDst->pObject->type != TYPE_ASTEROID) )
					continue;

				// check if the object rectangle overlap
				if (AETestRectToRect(
					&pSrc->posCurr, pSrc->scale, pSrc->scale, 
					&pDst->posCurr, pDst->scale, pDst->scale) == false)
					continue;

				// create the big explosion
				sparkCreate(PTCL_EXPLOSION_L, &pSrc->posCurr, 100, 0.0f, 2.0f * PI);
				
				// reset the ship position and direction
				AEVec2Zero( &pSrc->posCurr );
				AEVec2Zero( &pSrc->velCurr );
				pSrc->dirCurr = 0.0f;

				sSpecialCtr = SHIP_SPECIAL_NUM;

				// destroy all asteroid near the ship so that you do not die as soon as the ship reappear
				for (u32 j = 0; j < GAME_OBJ_INST_NUM_MAX; j++)
				{
					GameObjInst* pInst = sGameObjInstList + j;
					AEVec2		 u;

					// skip no-active and non-asteroid object
					if (((pInst->flag & FLAG_ACTIVE) == 0)
                        || (pInst->pObject->type != TYPE_ASTEROID))
						continue;

					AEVec2Sub(&u, &pInst->posCurr, &pDst->posCurr);

					if (AEVec2Length(&u) < (pDst->scale * 2.0f))
					{
						sparkCreate( PTCL_EXPLOSION_M, &pInst->posCurr, 10, -PI, PI );
						gameObjInstDestroy( pInst );
					}
				}

				// reduce the ship counter
				//sShipCtr--;
				
                /*
				// if counter is less than 0, game over
				if (sShipCtr < 0)
				{
					sGameStateChangeCtr = 2.0;
					gameObjInstDestroy(spShip);
					spShip = 0;
				}
                */

				break;
			}
		}
	}

	// =====================================
	// calculate the matrix for all objects
	// =====================================

	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		AEMtx33		 m;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		AEMtx33Scale		(&pInst->transform, pInst->scale,     pInst->scale);
		AEMtx33Rot			(&m,                pInst->dirCurr);
		AEMtx33Concat		(&pInst->transform, &m,               &pInst->transform);
		AEMtx33Trans		(&m,                pInst->posCurr.x, pInst->posCurr.y);
		AEMtx33Concat		(&pInst->transform, &m,               &pInst->transform);
	}

    scores.clear();
    
    for( UserTable::iterator userIt = users.begin(); userIt != users.end(); ++userIt )
    {
        UserInfo *user = userIt->second;

        ResultStatus result;

        result.name_ = user->m_username;
        result.score_ = user->m_p_obj->score;

        scores.push_back( result );
    }
    
}

// ---------------------------------------------------------------------------

void GameStatePlayDraw(void)
{
	s8 strBuffer[1024];

	// draw all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		AEGfxSetTransform	(&sGameObjInstList[i].transform);
		AEGfxTriDraw		(sGameObjInstList[i].pObject->pMesh);
	}

    for( u32 i = 0; i < scores.size(); ++i )
    {
        u32 cur_score = scores[i].score_;
        Username &cur_name = scores[i].name_;
        sprintf(strBuffer, "%s: %u", cur_name.name_, cur_score );
	    AEGfxPrint( 10, i * 20 + 10, -1, strBuffer);
    }


    sprintf(strBuffer, "Time: %.2f", GAME_TIME_SEC - live_time );
    AEGfxPrint( 340, 10, -1, strBuffer);

/*
	sprintf(strBuffer, "Level: %d", AELogBase2(sAstNum));
	AEGfxPrint(10, 30, -1, strBuffer);

	sprintf(strBuffer, "Ship Left: %d", sShipCtr >= 0 ? sShipCtr : 0);
	AEGfxPrint(600, 10, -1, strBuffer);

	sprintf(strBuffer, "Special:   %d", sSpecialCtr);
	AEGfxPrint(600, 30, -1, strBuffer);
*/

	// display the game over message
	if (sShipCtr < 0)
		AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");

}

// ---------------------------------------------------------------------------

void GameStatePlayFree(void)
{

    try
    {
        p_main_socket->Shutdown();
    }
    catch( iSocket::SockErr e )
    {
        e.Print();
    }


    try
    {
        p_main_socket->Close();
    }
    catch( iSocket::SockErr e )
    {
        e.Print();
    }

    delete p_main_socket;

    try
    {
        CloseWinSock();
    }
    catch( WSErr e )
    {
        e.Print();
    }


	// kill all object in the list
	for (u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		gameObjInstDestroy(sGameObjInstList + i);

	// reset asteroid count
	sAstCtr = 0;
}

// ---------------------------------------------------------------------------

void GameStatePlayUnload(void)
{
	// free all mesh
	for (u32 i = 0; i < sGameObjNum; i++)
		AEGfxTriFree(sGameObjList[i].pMesh);
}


void ProcMessage( NetworkMessage &netMessage )
{
    switch( netMessage.type_ )
    {
    case NetMsg::JOIN:
    {
        static u32 id = 0;

        if( id == 0 )
        {
            counting_down = true;
            init_time = AEGetTime();
        }

        MsgJoin j_msg;
        netMessage >> j_msg;
        
        std::string username = std::string( j_msg.data_.username_.name_ );
        
        UserTable::iterator userIt = users.find( username );

        if( userIt != users.end() )
        {
            printf( "User %s, %u attempted to rejoin\n", username.c_str(), userIt->second->m_id );
            return;
        }

        UserInfo *p_u_info = new UserInfo();
        p_u_info->m_address = netMessage.receiverAddress_;
        p_u_info->m_id = id++;
        p_u_info->m_username = j_msg.data_.username_;
        p_u_info->m_p_obj = CreateNewShip();

        users[ username ] = p_u_info;

        printf( "User %s joined and was assigned id %u\n", username.c_str(), p_u_info->m_id );

        break;
    }
    case NetMsg::INPUT:
    {
        MsgInput i_msg;

        netMessage >> i_msg;

        std::string username( i_msg.data_.username_.name_ );
        UserTable::iterator userIt = users.find( username );

        if( userIt == users.end() )
        {
            printf( "Error: input from invalid user %s\n", username.c_str() );
            return;
        }

        u32 count = i_msg.data_.key_info_count_;
        for( u32 i = 0; i < count; ++i )
        {
            KeyInfo key_info = i_msg.data_.key_data_[i];

            InputInfo info;
            info.m_key = key_info.key_;
            info.m_key_state = key_info.state_;
            info.m_p_user = userIt->second;
            
            inputs.push_back( info );
        }


        break;
    }
    default:
        break;
    }
}


void ProcInput( void )
{
	// =================
	// update the input
	// =================

    for( InputList::iterator inputIt = inputs.begin(); inputIt != inputs.end(); ++inputIt )
    {

    GameObjInst *spShip = inputIt->m_p_user->m_p_obj;

	if (spShip == 0)
	{
		sGameStateChangeCtr -= (f32)(gAEFrameTime);

		if (sGameStateChangeCtr < 0.0)
			gGameStateNext = GS_RESULT;
	}
	else
	{
		if ( inputIt->m_key_state == KEY_DOWN )
        {
		if ( inputIt->m_key == DIK_UP )
		{
			AEVec2 pos, dir;

			AEVec2Set	(&dir, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			pos = dir;
			AEVec2Scale	(&dir, &dir, SHIP_ACCEL_FORWARD * (f32)(gAEFrameTime));
			AEVec2Add	(&spShip->velCurr, &spShip->velCurr, &dir);
			AEVec2Scale	(&spShip->velCurr, &spShip->velCurr, pow(SHIP_DAMP_FORWARD, (f32)(gAEFrameTime)));

			AEVec2Scale	(&pos, &pos, -spShip->scale);
			AEVec2Add	(&pos, &pos, &spShip->posCurr);

#if !NO_SPARKS
			sparkCreate(PTCL_EXHAUST, &pos, 2, spShip->dirCurr + 0.8f * PI, spShip->dirCurr + 1.2f * PI);
#endif
        }
		if ( inputIt->m_key == DIK_DOWN )
		{
			AEVec2 dir;
			
			AEVec2Set	(&dir, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			AEVec2Scale	(&dir, &dir, SHIP_ACCEL_BACKWARD * (f32)(gAEFrameTime));
			AEVec2Add	(&spShip->velCurr, &spShip->velCurr, &dir);
			AEVec2Scale	(&spShip->velCurr, &spShip->velCurr, pow(SHIP_DAMP_BACKWARD, (f32)(gAEFrameTime)));
		}
		if ( inputIt->m_key == DIK_LEFT )
		{
			sShipRotSpeed   += (SHIP_ROT_SPEED - sShipRotSpeed) * 0.1f;
			spShip->dirCurr += sShipRotSpeed * (f32)(gAEFrameTime);
			spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
		}
		else if ( inputIt->m_key == DIK_RIGHT )
		{
			sShipRotSpeed   += (SHIP_ROT_SPEED - sShipRotSpeed) * 0.1f;
			spShip->dirCurr -= sShipRotSpeed * (f32)(gAEFrameTime);
			spShip->dirCurr =  AEWrap(spShip->dirCurr, -PI, PI);
		}
		else
		{
			sShipRotSpeed = 0.0f;
		}
        }
        else if( inputIt->m_key_state == KEY_TRIGGERED )
        {
	    if ( inputIt->m_key == DIK_SPACE )
		{
			AEVec2 vel;

			AEVec2Set	(&vel, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			AEVec2Scale	(&vel, &vel, BULLET_SPEED);
			
			gameObjInstCreate(TYPE_BULLET, 1.0f, &spShip->posCurr, &vel, spShip->dirCurr, true, spShip );
		}
	    if ( inputIt->m_key == DIK_Z && ( sSpecialCtr >= BOMB_COST ) )
		{
			u32 i;

			// make sure there is no bomb is active currently
			for (i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
				if ((sGameObjInstList[i].flag & FLAG_ACTIVE) && (sGameObjInstList[i].pObject->type == TYPE_BOMB))
					break;

			// if no bomb is active currently, create one
			if (i == GAME_OBJ_INST_NUM_MAX)
			{
				sSpecialCtr -= BOMB_COST;
				gameObjInstCreate(TYPE_BOMB, 1.0f, &spShip->posCurr, 0, 0, true);
			}
		}
	    if ( inputIt->m_key == DIK_X && (sSpecialCtr >= MISSILE_COST) )
		{
			sSpecialCtr -= MISSILE_COST;

			f32			 dir = spShip->dirCurr;
			AEVec2       vel = spShip->velCurr;
			AEVec2		 pos;

			AEVec2Set	(&pos, AECos(spShip->dirCurr), AESin(spShip->dirCurr));
			AEVec2Scale	(&pos, &pos, spShip->scale * 0.5f);
			AEVec2Add	(&pos, &pos, &spShip->posCurr);
			
			gameObjInstCreate(TYPE_MISSILE, 1.0f, &pos, &vel, dir, true);
		}
        if ( inputIt->m_key == DIK_Q )
        {
            std::string name = std::string( inputIt->m_p_user->m_username.name_ );
            UserTable::iterator userIt = users.find( name );

            userIt->second->m_p_obj->flag = 0;
            delete userIt->second;

            users.erase( userIt );
        }
        }
	}
    }

    inputs.clear();
}

void SendUpdate( void )
{
    MsgPosUpdate update_msg;

    update_msg.data_.time_left_ = GAME_TIME_SEC - live_time;
    // comment
    u32 active_count = 0;
    NetworkObjInst *p_cur_inst = ( NetworkObjInst * ) update_msg.data_.inst_data_;
    for( u32 i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i )
    {
        GameObjInst &game_obj = sGameObjInstList[i];
        if( ( game_obj.flag & FLAG_ACTIVE ) == 0 )
            continue;

        NetworkObjInst net_inst;
        net_inst.type = game_obj.type;
        net_inst.life = game_obj.life;
        net_inst.scale = game_obj.scale;
        net_inst.posCurr = game_obj.posCurr;
        net_inst.velCurr = game_obj.velCurr;
        net_inst.dirCurr = game_obj.dirCurr;
 
        *p_cur_inst = net_inst;
        ++active_count;
        ++p_cur_inst;
    }
    
    u32 msg_size = active_count * sizeof( NetworkObjInst );
    update_msg.data_.inst_count_ = active_count;

    NetMsg net_msg;

    net_msg << update_msg;
    net_msg.type_ = NetMsg::POS_UPDATE;

    SendToAll( net_msg );
}

void SendScores( bool final )
{
    MsgOutcome out_msg;
    out_msg.data_.num_players_ = scores.size();
    out_msg.data_.final_ = final;

    u32 i = 0;
    for( ScoreList::iterator scoreIt = scores.begin(); scoreIt != scores.end(); ++scoreIt )
    {
        out_msg.data_.status_data_[i] = *scoreIt;
        ++i;
    }

    NetworkMessage net_msg;
    net_msg << out_msg;
    SendToAll( net_msg );
}

void SendToAll( NetworkMessage &net_msg )
{
    for( UserTable::iterator userIt = users.begin(); userIt != users.end(); ++userIt )
    {
        SocketAddress to_addr = userIt->second->m_address;
        net_msg.receiverAddress_ = to_addr;
        p_main_socket->Send( net_msg );
    }
}

