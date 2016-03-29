#ifndef __GLOBALS_H
#define __GLOBALS_H


#include <jsmn.h>
#define STRING_TYPE(X) (X == sizeof(char) ? "Char"  \
						: (X == sizeof(int) ? "Int"  \
						: (X == sizeof(jsmntok_t) ? "Jsmn" \
						: "Unkown" ) ) ) 
/**< Returns string of the type of size X*/

#define SIGN_BIT 0xF000

typedef struct vec2_s
{
	int x;
	int y;
}vec2_t;

extern vec2_t gZeroPos;

typedef unsigned int address_t;
typedef struct KV_Pair_s
{
	char *key;
	address_t value;
}KV_Pair_t;

void Vec2Add(vec2_t *A, vec2_t *B, vec2_t *C);
void Vec2Subtract(vec2_t *First, vec2_t *Second, vec2_t *C);
void Vec2MultiplyScalar(vec2_t *A, int B, vec2_t *C);
extern int LargestDivisor(int num);
//#define VEC2_ADD(A, B, C) (C[0] = A[0] + B[0], C[1] = A[1] + B[1]) 
//#define VEC2_SUBTRACT(A, B, C) (C[0] = A[0] - B[0], C[1] = A[1] - B[1])


typedef enum
{
	ENTITY_STATE_DEAD,
	ENTITY_STATE_ALIVE,
	ENTITY_STATE_OTHER
}entity_state_t;

typedef enum
{
	COLLISION_TYPE_STATIC,
	COLLISION_TYPE_RAGDOLL,
	COLLISION_TYPE_CLIP
}collision_type_t;

typedef enum
{
	MENU_ITEM_STATE_NULL,
	MENU_ITEM_STATE_SELECTED = 0x1,
	MENU_ITEM_STATE_NOT_SELECTED = 0x2,
	MENU_ITEM_STATE_PICKED = 0x4,
	MENU_ITEM_STATE_MAX = 0x8
}menu_item_state_t;

typedef enum
{
	ANIMATION_IDLE,
	ANIMATION_WALK,
	ANIMATION_JUMP,
	ANIMATION_ATTACK,
	ANIMATION_HIT,
	ANIMATION_MAX = 20

}animation_state_t;

typedef enum
{
	SPLASH =	0x1,
	START =		0x2,
	GUESS =		0x4,
	CHOOSE =	0x8,
	PLAYING =	0x10,
	END =		0x20,
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