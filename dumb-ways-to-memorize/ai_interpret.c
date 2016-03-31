#include "ai_interpret.h"
#include "entity.h"
#include "parseentity.h"
#include "parsepowerup.h"
#include "mystrings.h"
#include <stdio.h>
#include <stdlib.h>

ai_function_t * gVariableAIs = NULL;
ai_function_t * gPresetAIs = NULL;

char *gAI_Variables[] = {"speed", "frames", "time", "damage", 0};
char *gAI_Actions[] = {"nothing", "move", "walk", "jump", "attack", 0};
char *gAI_Conditions[] = {"distance_player", "distance_object", "object_check", "link_ai", "link_action", 0};

/**
 * AI that does nothing for its think, except make sure values are set.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void NothingAI(entity_t *ent)
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
	ent->mNextThink = gCurrentTime + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;

	//Check Data
	if( --ent->mData->mVariables[AI_VAR_TIME] == 0)
	{
		ent->mData->mVariables[AI_VAR_TIME] = 1;
		ent->mData = ent->mData->mLink;
	}
}

/**
 * AI to Move ent from point A to B w/o collisions.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 * 
 * @Todo	Learn how to lerp.
 */

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
	ent->mNextThink = gCurrentTime + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;
	
	//Move
	temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
	temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
	//TODO: normalize temp_vec2
	Vec2MultiplyScalar(&temp_vec2,ent->mData->mVariables[AI_VAR_SPEED],&temp_vec2);
	Vec2Add(&temp_vec2, &ent->mVelocity, &ent->mVelocity);

	//Check Data
	if( --ent->mData->mVariables[AI_VAR_TIME] == 0)
	{
		ent->mData->mVariables[AI_VAR_TIME] = 1;
		ent->mData = ent->mData->mLink;
	}
	

}

/**
 * AI to "walk" entity from point A to B w/ collisions.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 * 
 * @todo	Lerp
 */

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
	ent->mNextThink = gCurrentTime + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;

	//Move
	temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
	temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
	//TODO: normalize temp_vec2
	Vec2MultiplyScalar(&temp_vec2,ent->mData->mVariables[AI_VAR_SPEED],&temp_vec2);
	Vec2Add(&temp_vec2, &ent->mVelocity, &ent->mVelocity);

	//Check Data
	if( --ent->mData->mVariables[AI_VAR_TIME] == 0)
	{
		ent->mData->mVariables[AI_VAR_TIME] = 1;
		ent->mData = ent->mData->mLink;
	}
}

/**
 * AI to change velocity of the entity, defaults to a jumping movement.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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
	ent->mNextThink = gCurrentTime + ent->mData->mVariables[AI_VAR_FRAMES]*FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	ent->mWeight = (flags & AI_FLAG_GRAVITY) ? 0 : FindCachedEntity(ent->mName)->mWeight;

	//Move
	if(ent->mVelocity.y == 0)
	{
		temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
		temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
		//TODO: normalize temp_vec2
		Vec2MultiplyScalar(&temp_vec2,ent->mData->mVariables[AI_VAR_SPEED],&temp_vec2);
		if( (ent->mVelocity.x == temp_vec2.x) & (ent->mVelocity.y == temp_vec2.y) )
		{
			;
		} else
		{
			Vec2Add(&temp_vec2, &ent->mVelocity, &ent->mVelocity);
		}
		
	}

	//Check Data
	if( --ent->mData->mVariables[AI_VAR_TIME] == 0)
	{
		ent->mData->mVariables[AI_VAR_TIME] = 1;
		ent->mData = ent->mData->mLink;
	}
}

/**
 * AI for entity attacks, "spawns" a hit box / entity in front of Entity
 * 
 * @note	Currently Untested
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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

	if(ent->mData->mObject)
	{
		temp_ent = FindCachedEntity(ent->mData->mObject);
		if(temp_ent)
		{
			temp_ent->mVelocity.x = ent->mData->mVariables[AI_VAR_DIR_X];
			temp_ent->mVelocity.y = ent->mData->mVariables[AI_VAR_DIR_Y];
			Spawn(ent, temp_ent);
		}
		
	}

	//Check Data
	if( --ent->mData->mVariables[AI_VAR_TIME] == 0)
	{
		ent->mData->mVariables[AI_VAR_TIME] = 1;
		ent->mData = ent->mData->mLink;
	}
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
		return NothingAI;
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
	int i, j, k,children, position, variables_i, gravity;
	ai_function_t *retVal;
	jsmntok_t *temp_tok, *action_tok;
	object_t *temp_obj, *action_obj, *variables_obj;
	char *temp_str, *type_str, *cond_str,**variables_str;
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
		printf("No thinks in ai : %s \n", obj->name);
		return NULL;
	}

	type_str = FindValue(obj, AI_TYPE_STR, g_str);
	if(!type_str)
	{
		printf("No types in ai : %s \n", obj->name);
		return NULL;
	}

	children = CountMem(temp_obj->children, sizeof(object_t));
	retVal = (ai_function_t*) malloc(sizeof(ai_function_t)*(children+1));
	memset(retVal, 0, sizeof(ai_function_t)*(children+1));
	for(i = 0; i < children; i++)
	{
		for(j = 0; gAI_Variables[j]; j++ )
		{
			temp_str = FindValue(&temp_obj->children[i], gAI_Variables[j], g_str);
			SetAI_Var(&retVal[i], variables[StrToInt(temp_str)], (ai_variables_t)j );
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		for(j = 0; gAI_Actions[j]; j++ )
		{	
			action_obj = FindObject(&temp_obj->children[i], gAI_Actions[j]);
			action_tok = FindKey(temp_obj->children[i].keys, gAI_Actions[j], g_str);
			if(!action_obj && !action_tok)
			{
				continue;
			}

			SetAI_Action(&retVal[i],action_obj, action_tok, g_str, (ai_actions_t)j );
		}
		for(j = 0; gAI_Conditions[j]; j++)
		{
			cond_str = FindValue(&temp_obj->children[i], gAI_Conditions[j], g_str);
			if(!cond_str)
			{
				continue;
			}
			variables_obj = FindObject(&temp_obj->children[i], AI_VAR_STR);
			if(!variables_obj)
			{
				variables_str = NULL;
			} else
			{
				variables_i = CountObjectMembers(variables_obj, g_str);
				variables_str = (char**) malloc(sizeof(char*)*(variables_i+1));
				variables_str[variables_i] = NULL;
				for(k = 0; k < variables_i; k++)
				{
					temp_str = JsmnToString(&variables_obj->values[k], g_str);
					variables_str[k] = temp_str ? variables[StrToInt(temp_str)] : NULL;
					if(temp_str) free(temp_str);
					temp_str = NULL;
				}
			}

			SetAI_Check(&retVal[i], variables_str, variables[StrToInt(cond_str)], (ai_conditions_t)j );
			if(cond_str) free(cond_str);
			cond_str = NULL;
		}
		retVal[i].mFlags |= gravity ? AI_FLAG_GRAVITY : 0;
		retVal[i].mType = StrToAI_Type(type_str);
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
			temp_str = FindValue(&temp_obj->children[i], gAI_Conditions[AI_CONDITION_OBJECT_NAME], g_str);
			if(!temp_str)
			{
				continue;
			}
			retVal[i].mObjectCheck = temp_str;
		}
	}
	return retVal;
}

ai_function_t* ParsePresetAI(object_t* obj, char* g_str)
{
	int i, j, k,children, position, variables, gravity;
	ai_function_t *retVal;
	jsmntok_t *temp_tok, *action_tok;
	object_t *temp_obj, *action_obj, *variables_obj;
	char *temp_str, *type_str, **variables_str;
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
		printf("No thinks in ai : %s \n", obj->name);
		return NULL;
	}
	temp_tok = FindKey(obj->keys, AI_TYPE_STR, g_str);
	if(!temp_tok)
	{
		printf("No types in ai : %s \n", obj->name);
		return NULL;
	}
	position = temp_tok - obj->keys;
	type_str = JsmnToString(&obj->values[position], g_str);
	
	children = CountMem(temp_obj->children, sizeof(object_t));
	retVal = (ai_function_t*) malloc(sizeof(ai_function_t)*(children+1));
	memset(retVal, 0, sizeof(ai_function_t)*(children+1));
	for(i = 0; i < children; i++)
	{
		for(j = 0; gAI_Variables[j]; j++ )
		{
			temp_str = FindValue(&temp_obj->children[i], gAI_Variables[j] ,g_str);
			
			SetAI_Var(&retVal[i], temp_str, (ai_variables_t)j );
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		for(j = 0; gAI_Actions[j]; j++ )
		{	
			action_obj = FindObject(&temp_obj->children[i], gAI_Actions[j]);
			action_tok = FindKey(temp_obj->children[i].keys, gAI_Actions[j], g_str);
			if(!action_obj && !action_tok)
			{
				continue;
			}
			SetAI_Action(&retVal[i],action_obj, action_tok, g_str, (ai_actions_t)j );
		}
		for(j = 0; gAI_Conditions[j]; j++)
		{
			temp_str = FindValue(&temp_obj->children[i], gAI_Conditions[j] ,g_str);
			if(!temp_str)
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

			SetAI_Check(&retVal[i], variables_str, temp_str, (ai_conditions_t)j );
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		retVal[i].mFlags |= gravity ? AI_FLAG_GRAVITY : 0;
		retVal[i].mType = StrToAI_Type(type_str);
	}

	//Linking and conditionals
	for(i = 0; i < children; i++)
	{
		if(!retVal[i].mLink)
		{
			retVal[i].mLink = (i+1 == children) ? retVal: &retVal[i+1];
		}
		if(retVal[i].mFlags & AI_FLAG_CHECK_OBJECT)
		{
			temp_str = FindValue(&temp_obj->children[i], gAI_Conditions[AI_CONDITION_OBJECT_NAME], g_str);
			if(!temp_str)
			{
				continue;
			}
			retVal[i].mObjectCheck = temp_str;
		}
	}
	return retVal;
}

void SetAI_Var(ai_function_t* function, char* data_str, ai_variables_t var_type)
{
	if(!var_type)
	{
		return;
	}

	switch(var_type)
	{
	case AI_VAR_SPEED:
		{
			function->mVariables[AI_VAR_SPEED] = data_str ? StrToInt(data_str) : AI_BASE_SPEED;
			break;
		}
	case AI_VAR_FRAMES:
		{
			function->mVariables[AI_VAR_FRAMES] = data_str ? StrToInt(data_str) : AI_BASE_THINK_FRAMES;
			break;
		}
	case AI_VAR_TIME:
		{
			function->mVariables[AI_VAR_TIME] = data_str ? StrToInt(data_str) : 1;
			break;
		}
	case AI_VAR_DAMAGE:
		{
			function->mVariables[AI_VAR_DAMAGE] = data_str ? StrToInt(data_str) : AI_BASE_DAMAGE;
			break;
		}
	default:
		break;
	}

}

void SetAI_Action(ai_function_t* function, object_t* obj, jsmntok_t* tok, char* g_str, ai_actions_t action_type)
{
	vec2_t *temp_vec2;
	if(!action_type || !function)
	{
		return;
	}
	switch(action_type)
	{
	case AI_ACTION_NOTHING:
		{
			function->mAction = AI_ACTION_NOTHING;
			break;
		}
	case AI_ACTION_MOVE:
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
				break;
			}
	case AI_ACTION_WALK:
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
			break;
		}
	case AI_ACTION_JUMP:
		{
			if(!obj)
			{
				function->mVariables[AI_VAR_DIR_X] = 0;
				function->mVariables[AI_VAR_DIR_Y] = AI_BASE_JUMP;
			
			} else if( (temp_vec2 = ParseToVec2(obj, g_str)) != NULL)
			{
				function->mVariables[AI_VAR_DIR_X] = temp_vec2->x;
				function->mVariables[AI_VAR_DIR_X] = temp_vec2->y;
			} else
			{
				function->mVariables[AI_VAR_DIR_X] = 0;
				function->mVariables[AI_VAR_DIR_Y] = AI_BASE_JUMP;
			}

			function->mAction = AI_ACTION_JUMP;
			break;
		}
	case AI_ACTION_ATTACK:
		{
			function->mAction = AI_ACTION_ATTACK;
			function->mObject = JsmnToString(tok, g_str);
			break;
		}
	default:
		break;
	}
}

void SetAI_Check(ai_function_t* function, char** variables_str, char* data_str, ai_conditions_t condition)
{
	ai_function_t *temp_ai;
	object_t *temp_obj;
	jsmntok_t *temp_tok;
	char *temp_str;
	if(!condition || !function)
	{
		return;
	}
	switch(condition)
	{
	case AI_CONDITION_LINK_ACTION:
		{
			function->mVariables[AI_VAR_CHECK] = StrToInt(data_str);
			break;
		}
	case AI_CONDITION_PLAYER_DISTANCE:
		{
			function->mVariables[AI_VAR_CHECK] = StrToInt(data_str);
			function->mFlags |= AI_FLAG_CHECK_PLAYER;
			break;
		}
	case AI_CONDITION_OBJECT_DISTANCE:
		{
			function->mVariables[AI_VAR_CHECK] = StrToInt(data_str);
			function->mFlags |= AI_FLAG_CHECK_OBJECT;
			break;
		}
	case AI_CONDITION_LINK_AI:
		{
			ConvertFileToUseable(data_str, NULL, &temp_str, &temp_tok);
			if(!temp_str || !temp_tok)
			{
				return;
			}
			temp_obj = ParseToObject(temp_tok, temp_str);
			function->mLink = ParseAI(temp_obj, temp_str, variables_str);
			break;
		}
	default:
		break;
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
	atexit(ShutdownAISystem);
	return 0;
}

void ShutdownAISystem()
{
	if(!gVariableAIs || !gPresetAIs)
	{
		printf("AI not initialized before shutdown called \n");
		return;
	}
	free(gVariableAIs);
	free(gPresetAIs);
}

ai_type_t StrToAI_Type(const char *str)
{
	if(!strcmp(str, AI_TYPE_PRESET_STR))
	{
		return AI_TYPE_PRESET;
	} else if(!strcmp(str, AI_TYPE_VARIABLE_STR))
	{
		return AI_TYPE_VARIABLE;
	}
	return AI_TYPE_NULL;
}

ai_actions_t StrToAI_Action(const char* str)
{
	int i;
	for(i = 0; gAI_Actions[i]; i++)
	{
		if(!strcmp(gAI_Actions[i], str))
		{
			return (ai_actions_t)i;
		}
	}
	return  AI_ACTION_MAX;
}

ai_conditions_t StrToAI_Condition(const char* str)
{
	int i;
	for(i = 0; gAI_Conditions[i]; i++)
	{
		if(!strcmp(gAI_Conditions[i], str))
		{
			return (ai_conditions_t)i;
		}
	}
	return  AI_CONDITION_MAX;
}

ai_variables_t StrToVariableType(const char* str)
{
	int i;
	for(i = 0; gAI_Variables[i]; i++)
	{
		if(!strcmp(gAI_Variables[i], str))
		{
			return (ai_variables_t)i;
		}
	}
	return  AI_VAR_MAX;
}
