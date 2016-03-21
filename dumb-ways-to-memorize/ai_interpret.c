#include "ai_interpret.h"
#include "entity.h"
#include "parsevariable.h"
#include "mystrings.h"
#include <stdio.h>

ai_function_t * gVariableAIs = NULL;
ai_function_t * gPresetAIs = NULL;

char *gAI_Variables[] = {"speed", "frames", "time", "damage", 0};
char *gAI_Actions[] = {"nothing", "move", "walk", "jump","attack",0};
char *gAI_Conditions[] = {"distance_player", "distance_object", "link_ai", "link_action", 0};

//Check Data Flag checks before calling functions
//TODO: Lerp
void MoveAI(entity_t *ent)
{
	int flags;
	vec2_t temp_vec2;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}
	flags = ent->mData->mFlags;

	//Standard Vars
	ent->mCollisionType = COLLISION_TYPE_CLIP;
	ent->mNextThink = SDL_GetTicks() + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;
	
	//Move
	temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
	temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
	//TODO: normalize temp_vec2
	Vec2MultiplyScalar(&temp_vec2,ent->mData->mVariables[AI_VAR_SPEED],&temp_vec2);
	Vec2Add(&temp_vec2, &ent->mVelocity, &ent->mVelocity);

	//Check Data
	ent->mData = ent->mData->mVariables[4]-- ? ent->mData : ent->mData->mLink;

}

//TODO: Lerp
void WalkAI(entity_t *ent)
{
	int flags;
	vec2_t temp_vec2;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}
	flags = ent->mData->mFlags;

	//Standard Vars
	ent->mCollisionType = COLLISION_TYPE_RAGDOLL;
	ent->mNextThink = SDL_GetTicks() + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;

	//Move
	temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
	temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
	//TODO: normalize temp_vec2
	Vec2MultiplyScalar(&temp_vec2,ent->mData->mVariables[AI_VAR_SPEED],&temp_vec2);
	Vec2Add(&temp_vec2, &ent->mVelocity, &ent->mVelocity);

	//Check Data
	ent->mData = ent->mData->mVariables[4]-- ? ent->mData : ent->mData->mLink;

}

void JumpAI(entity_t *ent)
{
	int flags;
	vec2_t temp_vec2;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}
	flags = ent->mData->mFlags;

	//Standard Vars
	ent->mCollisionType = COLLISION_TYPE_RAGDOLL;
	ent->mNextThink = SDL_GetTicks() + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;

	//Move
	temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
	temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
	//TODO: normalize temp_vec2
	Vec2MultiplyScalar(&temp_vec2,ent->mData->mVariables[AI_VAR_SPEED],&temp_vec2);
	Vec2Add(&temp_vec2, &ent->mVelocity, &ent->mVelocity);

	//Check Data
	ent->mData = ent->mData->mVariables[4]-- ? ent->mData : ent->mData->mLink;
}

void AttackAI(entity_t *ent)
{
	int flags;
	vec2_t temp_vec2;
	entity_t *temp_ent;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}
	flags = ent->mData->mFlags;

	//Standard Vars
	ent->mCollisionType = COLLISION_TYPE_RAGDOLL;
	ent->mNextThink = SDL_GetTicks() + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;



	//Check Data
	ent->mData = ent->mData->mVariables[4]-- ? ent->mData : ent->mData->mLink;
}

void (*GetFunctionAI(ai_function_t *data))(entity_t *)
{
	if(!data)
	{
		return NULL;
	}
	switch(data->mAction)
	{
	case(AI_ACTION_NOTHING) : 
		return NULL;
	case(AI_ACTION_MOVE):
		return MoveAI;
	case(AI_ACTION_WALK): 
		return WalkAI;
	case(AI_ACTION_JUMP):
		return JumpAI;
	case(AI_ACTION_ATTACK):
		return AttackAI;
	default:
		return NULL;
	}
}

ai_function_t *ParseAI(object_t *obj, char *g_str, char **variables)
{
	return NULL;
}

ai_function_t* ParsePresetAI(object_t* obj, char* g_str)
{
	int i, j, k,children, position, variables, gravity;
	ai_function_t *retVal;
	jsmntok_t *temp_tok;
	object_t *temp_obj, *action_obj, *variables_obj;
	char *temp_str, **variables_str;
	if(!obj || !g_str)
	{
		return NULL;
	}
	
	if(FindObject(obj, AI_VAR_STR))
	{
		gravity = 1;
	} else
	{
		gravity = 0;
	}
	temp_obj = FindObject(obj, AI_FUNCTION_OBJECT);
	if(!temp_obj)
	{
		return NULL;
	}
	
	children = CountMem(temp_obj->children, sizeof(object_t));
	retVal = (ai_function_t*) malloc(sizeof(ai_function_t)*(children+1));
	memset(retVal, 0, sizeof(ai_function_t)*(children+1));
	for(i = 0; i < children; i++)
	{
		for(j = 0; gAI_Variables[j]; j++ )
		{

			temp_tok = FindKey(temp_obj->children[i].keys,gAI_Variables[j], g_str);
			position = temp_tok - temp_obj->children[i].keys ;
			if(position < 0)
			{
				temp_str = NULL;
			} else
			{
				temp_str = JsmnToString(&temp_obj->children[i].values[position], g_str);
			}
			
			SetAI_Var(&retVal[i], temp_str, gAI_Variables[j]);
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		for(j = 0; gAI_Actions[j]; j++ )
		{	
			action_obj = FindObject(&temp_obj->children[i], gAI_Actions[j]);
			if(!action_obj)
			{
				continue;
			}
			SetAI_Action(&retVal[i],action_obj, g_str, gAI_Actions[j]);
		}
		for(j = 0; gAI_Conditions[j]; j++)
		{
			temp_tok = FindKey(temp_obj->children[i].keys, gAI_Conditions[j], g_str);
			if(!temp_tok)
			{
				continue;
			}
			variables_obj = FindObject(&temp_obj->children[i], AI_VAR_STR);
			if(!variables_obj)
			{
				variables_str = NULL;
			} else
			{
				variables = CountObjectMembers(variables_obj, g_str);
				variables_str = (char**) malloc(sizeof(char*)*(variables+1));
				variables_str[variables] = NULL;
				for(k = 0; k < variables; k++)
				{
					variables_str[k] = JsmnToString(&variables_obj->values[k],g_str);
				}
			}
			position = temp_tok - temp_obj->children[i].keys;
			temp_str = JsmnToString(&temp_obj->children[i].values[position], g_str);

			SetAI_Check(&retVal[i], variables_str, temp_str, gAI_Conditions[j]);
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		retVal->mFlags |= gravity ? AI_FLAG_GRAVITY : 0;
	}

	//Linking and conditionals
	for(i = 0; i < children; i++)
	{
		if(retVal[i].mLink)
		{
			continue;
		} else
		{
			retVal[i].mLink = (i+1 == children) ? retVal: &retVal[i+1];
		}
		if(retVal[i].mFlags & AI_FLAG_CHECK_OBJECT)
		{
			temp_tok = FindKey(temp_obj->children[i].keys, AI_SPECIFY_OBJ_STR, g_str);
			if(!temp_tok)
			{
				continue;
			}
			position = temp_tok - temp_obj->children[i].keys;
			retVal[i].mObjectCheck = JsmnToString(&temp_obj->children[i].values[position], g_str);
		}
	}
}

void SetAI_Var(ai_function_t* function, char* data_str, char* var_str)
{
	if(!var_str)
	{
		return;
	}
	if(!strcmp(AI_VAR_SPEED_STR, var_str))
	{
		if(!data_str)
		{
			function->mVariables[AI_VAR_SPEED] = 1;
		} else
		{
			function->mVariables[AI_VAR_SPEED] = StringToInt(data_str);
		}
	}
	else if(!strcmp(AI_VAR_FRAMES_STR, var_str))
	{
		if(!data_str)
		{
			function->mVariables[AI_VAR_FRAMES] = 1;
		} else
		{
			function->mVariables[AI_VAR_FRAMES] = StringToInt(data_str);
		}
	}
	else if(!strcmp(AI_VAR_TIME_STR, var_str))
	{
		if(!data_str)
		{
			function->mVariables[AI_VAR_TIME] = 1;
		} else
		{
			function->mVariables[AI_VAR_TIME] = StringToInt(data_str);
		}
	}
	else if(!strcmp(AI_VAR_DAMAGE_STR, var_str))
	{
		if(!data_str)
		{
			function->mVariables[AI_VAR_DAMAGE] = 1;
		} else
		{
			function->mVariables[AI_VAR_DAMAGE] = StringToInt(data_str);
		}
	}
}

void SetAI_Action(ai_function_t* function, object_t* obj, char* g_str, char* action_str)
{
	vec2_t *temp_vec2;
	if(!action_str || !function)
	{
		return;
	}
	if(!strcmp(AI_ACTION_NOTHING_STR, action_str))
	{
		function->mAction = AI_ACTION_NOTHING;
	}

	else if(!strcmp(AI_ACTION_MOVE_STR, action_str))
	{
		if(!obj)
		{
			function->mVariables[AI_VAR_DIR_X] = 0;
			function->mVariables[AI_VAR_DIR_Y] = 0;
			
		} else if( (temp_vec2 = ParseToVec2(obj, g_str)) != NULL)
		{
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->x;
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->y;
		} else
		{
			function->mVariables[AI_VAR_DIR_X] = 0;
			function->mVariables[AI_VAR_DIR_Y] = 0;
		}

		function->mAction = AI_ACTION_MOVE;
	}

	else if(!strcmp(AI_ACTION_WALK_STR, action_str))
	{
		if(!obj)
		{
			function->mVariables[AI_VAR_DIR_X] = 0;
			function->mVariables[AI_VAR_DIR_Y] = 0;
			
		} else if( (temp_vec2 = ParseToVec2(obj, g_str)) != NULL)
		{
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->x;
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->y;
		} else
		{
			function->mVariables[AI_VAR_DIR_X] = 0;
			function->mVariables[AI_VAR_DIR_Y] = 0;
		}

		function->mAction = AI_ACTION_WALK;
	}

	else if(!strcmp(AI_ACTION_JUMP_STR, action_str))
	{
		if(!obj)
		{
			function->mVariables[AI_VAR_DIR_X] = 0;
			function->mVariables[AI_VAR_DIR_Y] = 0;
			
		} else if( (temp_vec2 = ParseToVec2(obj, g_str)) != NULL)
		{
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->x;
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->y;
		} else
		{
			function->mVariables[AI_VAR_DIR_X] = 0;
			function->mVariables[AI_VAR_DIR_Y] = 10;
		}

		function->mAction = AI_ACTION_JUMP;
	}
	else if(!strcmp(AI_ACTION_MOVE_STR, action_str))
	{
		function->mAction = AI_ACTION_ATTACK;
	}
}

void SetAI_Check(ai_function_t* function, char** variables_str, char* data_str, char* check_str)
{
	ai_function_t *temp_ai;
	object_t *temp_obj;
	jsmntok_t *temp_tok;
	char *temp_str;
	if(!check_str || !function)
	{
		return;
	}
	if(!strcmp(AI_LINK_ACTION, check_str) )
	{
		function->mVariables[AI_VAR_CHECK] = StringToInt(data_str);
	} 
	else if(!strcmp(AI_CHECK_PLAYER_STR, check_str))
	{
		function->mVariables[AI_VAR_CHECK] = StringToInt(data_str);
		function->mFlags |= AI_FLAG_CHECK_PLAYER;
	} 
	else if(!strcmp(AI_CHECK_OBJECT_STR, check_str))
	{
		function->mVariables[AI_VAR_CHECK] = StringToInt(data_str);
		function->mFlags |= AI_FLAG_CHECK_OBJECT;
	} 
	else if(!strcmp(AI_LINK_AI, check_str))
	{

		ConvertFileToUseable(data_str, NULL, &temp_str, &temp_tok);
		if(!temp_str || !temp_tok)
		{
			return;
		}
		temp_obj = ParseToObject(temp_tok, temp_str);
		function->mLink = ParseAI(temp_obj, temp_str, variables_str);
	}
}

int InitAISystem()
{
	if(gVariableAIs)
	{
		printf("Tried to init AI system, while already inited \n");
		return -1;
	}
	gVariableAIs = (ai_function_t*) malloc(sizeof(ai_function_t)*(MAX_AI+1));
	gPresetAIs = (ai_function_t*) malloc(sizeof(ai_function_t)*(MAX_AI+1));

	if(!gVariableAIs || !gPresetAIs)
	{
		printf("AI malloc error \n");
		return -1;
	}

	memset(gVariableAIs, 0, sizeof(ai_function_t)*(MAX_AI+1));
	memset(gPresetAIs, 0, sizeof(ai_function_t)*(MAX_AI+1));

	return 0;
}
