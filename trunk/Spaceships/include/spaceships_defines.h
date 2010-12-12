
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
