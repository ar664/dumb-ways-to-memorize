#ifndef __AI_INTERPRET_H
#define __AI_INTERPRET_H

#include "globals.h"
#include "parseobject.h"


#define MAX_AI					255

#define AI_FUNCTION_OBJECT		"Think"
#define AI_VAR_G_STR			"Variables"
#define AI_RECURSIVE_STR		"AI_Recursive"
#define AI_VAR_STR				"variables"
#define AI_VAR_FRAMES_STR		"frames"
#define AI_VAR_TIME_STR			"time"
#define AI_VAR_DAMAGE_STR		"damage"
#define AI_VAR_VELOCITY_STR		"velocity"
#define AI_VAR_GRAVITY_STR		"gravity"
#define AI_ACTION_NOTHING_STR	"nothing"
#define AI_ACTION_MOVE_STR		"move"
#define AI_ACTION_WALK_STR		"walk"
#define AI_ACTION_JUMP_STR		"jump:
#define AI_ACTION_ATTACK_STR	"attack"
#define AI_TYPE_STR				"AI_Type"
#define AI_TYPE_PRESET_STR		"preset"
#define AI_TYPE_VARIABLE_STR	"variable"



typedef enum
{
	AI_TYPE_NULL,
	AI_TYPE_PRESET = 0x1,
	AI_TYPE_VARIABLE,
	AI_TYPE_RECURSIVE
}ai_type_t;

typedef enum
{
	AI_FLAG_FRAMES	= 0x1,
	AI_FLAG_TIME	= 0x2,
	AI_FLAG_DAMAGE	= 0x4,
	AI_FLAG_VELOCITY= 0x8,
	AI_FLAG_GRAVITY	= 0x10,
	AI_FLAG_MAX		= 0x20,

}ai_variables_t;

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

struct ai_function_s
{
	ai_type_t mType;
	ai_actions_t mAction;
	int mFlags;
	int mFrames;
	int mTime;
	int mDamage;
	int mSpeed;
	KV_Pair_t *mVariables;
	ai_function_t *mLink;


	//void (*function) (ai_function_t *self,entity_t *ent);
	//void *((*function_list) (ai_function_t *self, entity_t *ent));

};

extern ai_function_t *gVariableAIs;
extern ai_function_t *gPresetAIs;

ai_function_t *ParseAI(object_t *obj, char *g_str);
int InitAISystem();
void ShutdownAISystem();

#endif