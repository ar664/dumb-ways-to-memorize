#ifndef __AI_INTERPRET_H
#define __AI_INTERPRET_H

#include "globals.h"
#include "parseobject.h"


#define MAX_AI					255
#define	AI_PATH					"AI/"
#define AI_FUNCTION_OBJECT		"Think"
#define AI_VAR_G_STR			"Variables"
#define AI_RECURSIVE_STR		"AI_Recursive"
#define AI_VAR_STR				"variables"
extern char *gAI_Variables[];
#define AI_VAR_FRAMES_STR		"frames"
#define AI_VAR_TIME_STR			"time"
#define AI_VAR_DAMAGE_STR		"damage"
#define AI_VAR_SPEED_STR		"speed"
#define AI_VAR_GRAVITY_STR		"gravity"
extern char *gAI_Actions[];
#define AI_ACTION_NOTHING_STR	"nothing"
#define AI_ACTION_MOVE_STR		"move"
#define AI_ACTION_WALK_STR		"walk"
#define AI_ACTION_JUMP_STR		"jump"
#define AI_ACTION_ATTACK_STR	"attack"
#define AI_TYPE_STR				"AI_Type"
#define AI_TYPE_PRESET_STR		"preset"
#define AI_TYPE_VARIABLE_STR	"variable"
#define AI_CHECK_STR			"condition"
extern char *gAI_Conditions[];
#define AI_CHECK_PLAYER_STR		"distance_player"
#define AI_CHECK_OBJECT_STR		"distance_object"
#define AI_SPECIFY_OBJ_STR		"object_check"
#define AI_LINK_ACTION			"link_action"
#define AI_LINK_AI				"link_ai"


typedef enum
{
	AI_TYPE_NULL,
	AI_TYPE_PRESET = 0x1,
	AI_TYPE_VARIABLE,
	AI_TYPE_RECURSIVE
}ai_type_t;

typedef enum
{
	
	AI_VAR_SPEED,
	AI_VAR_DIR_X,
	AI_VAR_DIR_Y,
	AI_VAR_FRAMES,
	AI_VAR_TIME,
	AI_VAR_DAMAGE,
	AI_VAR_CHECK,
	AI_VAR_MAX

}ai_variables_t;

typedef enum
{
	AI_FLAG_CHECK_PLAYER = 0x1,
	AI_FLAG_CHECK_OBJECT = 0x2,
	AI_FLAG_GRAVITY	= 0x4,

}ai_flags_t;

typedef enum
{
	AI_ACTION_NOTHING,
	AI_ACTION_MOVE,
	AI_ACTION_WALK,
	AI_ACTION_JUMP,
	AI_ACTION_ATTACK,
	AI_ACTION_MAX

}ai_actions_t;

typedef struct ai_function_s ai_function_t;

//mVariables
// 1 - speed
// [2 , 3] - direction
// 4 - frames
// 5 - time
// 6 - damage
// 7 - check

struct ai_function_s
{
	ai_type_t mType;
	ai_actions_t mAction;
	int mFlags;
	int mVariables[7];
	char *mObject;
	char *mObjectCheck;
	ai_function_t *mLink;


	//void (*function) (ai_function_t *self,entity_t *ent);
	//void *((*function_list) (ai_function_t *self, entity_t *ent));

};

extern ai_function_t *gVariableAIs;
extern ai_function_t *gPresetAIs;

ai_function_t *ParseAI(object_t *obj, char *g_str, char **variables);
ai_function_t *ParsePresetAI(object_t *obj, char *g_str);
void SetAI_Var(ai_function_t* function, char * data_str, char * var_str);
void SetAI_Action(ai_function_t* function, object_t * obj, jsmntok_t* tok, char* g_str, char * action_str);
void SetAI_Check(ai_function_t* function, char** variables_str, char * data_str, char * check_str);
//void *GetFunctionAI(ai_function_t *data);
int InitAISystem();
void ShutdownAISystem();

#endif