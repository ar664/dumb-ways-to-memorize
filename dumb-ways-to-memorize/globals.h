#ifndef __GLOBALS_H
#define __GLOBALS_H


#include <jsmn.h>
#define STRING_TYPE(X) (X == sizeof(char) ? "Char"  \
						: (X == sizeof(int) ? "Int"  \
						: (X == sizeof(jsmntok_t) ? "Jsmn" \
						: "Unkown" ) ) ) 
/**< Returns string of the type of size X*/

#define SIGN_BIT 0xF000

/** Defines an alias representing the 2D vector */
typedef struct vec2_s
{
	int x;
	int y;
}vec2_t;

extern vec2_t gZeroPos;

//Address of a value, basically generic typing int*
typedef unsigned int address_t;
//Key Value pair struct
typedef struct KV_Pair_s
{
	char *key;
	address_t value;
}KV_Pair_t;

//#define VEC2_ADD(A, B, C) (C[0] = A[0] + B[0], C[1] = A[1] + B[1]) 
//#define VEC2_SUBTRACT(A, B, C) (C[0] = A[0] - B[0], C[1] = A[1] - B[1])
void Vec2Add(vec2_t *A, vec2_t *B, vec2_t *C);
void Vec2Subtract(vec2_t *First, vec2_t *Second, vec2_t *C);
void Vec2MultiplyScalar(vec2_t *A, int B, vec2_t *C);
extern int LargestDivisor(int num);


/** Defines the enum relating to entity states */
typedef enum
{
	ENTITY_STATE_DEAD,  /**< An enum constant representing the entity state dead */
	ENTITY_STATE_ALIVE, /**< An enum constant representing the entity state alive option */
	ENTITY_STATE_OTHER  /**< An enum constant representing the entity state other, which is used for non-health based entities */
}entity_state_t;

/** Defines the enum which are the collision types for the physics engine */
typedef enum
{
	COLLISION_TYPE_STATIC,  /**< An enum constant representing the collision type static - which means it collides, but doesn't move */
	COLLISION_TYPE_RAGDOLL, /**< An enum constant representing the collision type ragdoll - which collides + moves */
	COLLISION_TYPE_CLIP		/**< An enum constant representing the collision type clip - which means it doesn't collide , but moves. Should be used for particle entities (maybe) */
}collision_type_t;

/** Defines the enum for menu item states */
typedef enum
{
	MENU_ITEM_STATE_NULL,
	MENU_ITEM_STATE_SELECTED = 0x1,		/**< An enum constant representing the menu item state selected - which means the player is hovering over this now */
	MENU_ITEM_STATE_NOT_SELECTED = 0x2, /**< An enum constant representing the menu item state not selected */
	MENU_ITEM_STATE_PICKED = 0x4,		/**< An enum constant representing the menu item state picked - which is a option that is checked */
	MENU_ITEM_STATE_MAX = 0x8
}menu_item_state_t;

/** Defines an enum for animation states in mSprites array */
typedef enum
{
	ANIMATION_IDLE,		/**< An enum constant representing the idle animation */
	ANIMATION_WALK,		/**< An enum constant representing the walk animation*/
	ANIMATION_JUMP,		/**< An enum constant representing the jump animation */
	ANIMATION_ATTACK,   /**< An enum constant representing the attack animation */
	ANIMATION_HIT,		/**< An enum constant representing the hit animation */
	ANIMATION_MAX = 20  /**< An enum constant representing the maximum animations */

}animation_state_t;

/** Defines the enum for the game states */
typedef enum
{
	SPLASH =	0x1,	/**< An enum constant representing the splash screen game state */
	START =		0x2,	/**< An enum constant representing the start screen game state */
	GUESS =		0x4,	/**< An enum constant representing the first time the player chooses the power_ups */
	CHOOSE =	0x8,	/**< An enum constant representing the state in which the player selects a power_up from the chosen power ups in guess */
	PLAYING =	0x10,   /**< An enum constant representing the playing game state, which is consistent of the level, enemy entities, and the player */
	END =		0x20,   /**< An enum constant representing the player has chosen to quit the game */
}GameState;

extern GameState gGameState;
extern GameState StrToGameState(char *str);
extern int StrToMenuType(char *str);

//For Hazards
#define HAZARD_NULL	0x0
#define HAZARD_MAX	(0x1 >> 31)
#define HAZARD_DAMAGE 10
#define HAZARD_STUN_FRAMES 10
extern char **Hazards_str;
extern int StrToHazard(char *str);

//For EntParsing
#define ENTITIES_FILE_STR	"Entities"
extern char *gEntitiesFile;
extern char *Collisions_str[];
extern char *EntityStates_str[];
collision_type_t StrToCollisionType(char *str);
entity_state_t StrToEntityState(char *str);

//For PowerUps
#define POWER_UPS_STR	"PowerUps"
extern char *gPowerUpsFile;
extern vec2_t *mousePos;
extern int *keyPower;
extern char **gSelectedPowerUps;
extern char **gUsedPowerUps;
extern char *gCurrentPowerUpName;

//For Levels
extern char **gLevels;
extern char **gSelectedLevels;
int SelectLevels();
int LoadSelectedLevel(int level);
void RandomizeSelectedLevels();

extern void *gWorld;
extern void *gMouse;

//Game Globals
#define G_NAME_STR		"Name"
#define LOC_NAME_STR	"name"
#define	FRAME_DELAY		13
#define PHYSICS_LIMITER 2
extern int gLives;
extern int gLevelsPerGame;
extern int gScreenWidth;
extern int gScreenHeight;
extern unsigned int gCurrentTime;
extern vec2_t gGravity;

extern int exitRequest;

//Memory Functions
extern int CountMem(void *src, int size_type);
extern int AllocateDynamic(void **dst, void *src, int size_type, int size);
extern int CompareMemToMemArray(void *mem, void *mem_array, int size_type, int size_array);

//Foward dec of object_s for global objects
struct object_s;
//Prepoccessor defed LINKS and STRICT_MODE
//JSON Parser
extern jsmn_parser gParser;
extern jsmntok_t *gGameTokens;
extern struct object_s *gGameObject;
extern jsmntok_t *gEntityTokens;
extern struct object_s *gEntityObject;
extern jsmntok_t *gLevelTokens;
extern struct object_s *gLevelObject;
extern char *gGameData;
extern char *gEntityData;
extern char *gLevelData;

#endif