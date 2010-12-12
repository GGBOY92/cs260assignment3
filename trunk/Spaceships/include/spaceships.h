
#include "Vector2.h"

class AEGfxTriList;

// ---------------------------------------------------------------------------
// Defines

#define GAME_OBJ_NUM_MAX			32			//The total number of different objects (Shapes)
#define GAME_OBJ_INST_NUM_MAX		2048		//The total number of different game object instances


#define SHIP_INITIAL_NUM			3			// initial number of ship lives
#define SHIP_SIZE					3.0f		// ship size
#define SHIP_ACCEL_FORWARD			50.0f		// ship forward acceleration (in m/s^2)
#define SHIP_ACCEL_BACKWARD			-100.0f		// ship backward acceleration (in m/s^2)
#define SHIP_ROT_SPEED				(1.8f * PI)	// ship rotation speed (degree/second)

#define BULLET_SPEED				100.0f		// bullet speed (m/s)
#define ROID_SIZE					20.0f
#define MAX_GENERATION				2

#define SCORE_ROID_DESTROYED        100
// ---------------------------------------------------------------------------
enum TYPE
{
	// list of game object types
	TYPE_SHIP = 0, 
	TYPE_BULLET,
	TYPE_ASTEROID,

	TYPE_NUM
};

// ---------------------------------------------------------------------------
// object flag definition

#define FLAG_ACTIVE		0x00000001

// ---------------------------------------------------------------------------
// Struct/Class definitions

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
