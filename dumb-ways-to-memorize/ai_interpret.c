#include "ai_interpret.h"
#include "entity.h"
#include "parsevariable.h"
#include "parsepowerup.h"
#include "mystrings.h"
#include <stdio.h>

ai_function_t * gVariableAIs = NULL;
ai_function_t * gPresetAIs = NULL;

char *gAI_Variables[] = {"speed", "frames", "time", "damage", 0};
char *gAI_Actions[] = {"nothing", "move", "walk", "jump","attack",0};
char *gAI_Conditions[] = {"distance_player", "distance_object", "link_ai", "link_action", 0};

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

/**
 * Returns a function pointer to the think function, given ai_function data.
 *
 * @param [in,out]	parameter1	If non-null, the first parameter.
 *
 * @return	null if it fails, else a GetFunctionAI(ai_function_t *data.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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

/**
 * Parses AI behavior using the variables given and the AI obj/file specified.
 *
 * @param [in,out]	obj		 	If non-null, the object.
 * @param [in,out]	g_str	 	If non-null, the string.
 * @param [in,out]	variables	If non-null, the variables.
 *
 * @return	null if it fails, else a pointer to an ai_function_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

ai_function_t *ParseAI(object_t *obj, char *g_str, char **variables)
{
	int i, j, k,children, position, variables_i, gravity;
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

			temp_tok = FindKey(temp_obj->children[i].keys,gAI_Variables[j], g_str);
			position = temp_tok - temp_obj->children[i].keys ;
			if(position < 0)
			{
				temp_str = NULL;
			} else
			{
				temp_str = JsmnToString(&temp_obj->children[i].values[position], g_str);
			}
			
			SetAI_Var(&retVal[i], variables[StrToInt(temp_str)], gAI_Variables[j]);
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
			position = action_tok - temp_obj->children[i].keys;
			position = position < 0 ? 0 : position;
			temp_str = JsmnToString(&temp_obj->children[i].values[position], g_str);
			if(StrToInt(temp_str))
			{
				SetAI_Action(&retVal[i], NULL, NULL, g_str, gAI_Actions[AI_ACTION_NOTHING] );
			}
			SetAI_Action(&retVal[i],action_obj, action_tok, g_str, gAI_Actions[j]);
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
			position = temp_tok - temp_obj->children[i].keys;
			temp_str = JsmnToString(&temp_obj->children[i].values[position], g_str);

			SetAI_Check(&retVal[i], variables_str, variables[StrToInt(temp_str)], gAI_Conditions[j]);
			if(temp_str) free(temp_str);
			temp_str = NULL;
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
			temp_tok = FindKey(temp_obj->children[i].keys, AI_SPECIFY_OBJ_STR, g_str);
			if(!temp_tok)
			{
				continue;
			}
			position = temp_tok - temp_obj->children[i].keys;
			retVal[i].mObjectCheck = JsmnToString(&temp_obj->children[i].values[position], g_str);
		}
	}
	return retVal;
}

/**
 * Parse a preset AI, with values determined by whats in the file.
 *
 * @param [in,out]	obj  	If non-null, the object.
 * @param [in,out]	g_str	If non-null, the string.
 *
 * @return	null if it fails, else a pointer to an ai_function_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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
			
			SetAI_Var(&retVal[i], temp_str, gAI_Variables[j]);
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
			SetAI_Action(&retVal[i],action_obj, action_tok, g_str, gAI_Actions[j]);
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

			SetAI_Check(&retVal[i], variables_str, temp_str, gAI_Conditions[j]);
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
			temp_tok = FindKey(temp_obj->children[i].keys, AI_SPECIFY_OBJ_STR, g_str);
			if(!temp_tok)
			{
				continue;
			}
			position = temp_tok - temp_obj->children[i].keys;
			retVal[i].mObjectCheck = JsmnToString(&temp_obj->children[i].values[position], g_str);
		}
	}
	return retVal;
}

/**
 * Sets AI variables in ai_function->mVariables.
 *
 * @param [in,out]	function	If non-null, the function.
 * @param [in,out]	data_str	If non-null, the data string.
 * @param [in,out]	var_str 	If non-null, the variable string.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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
			function->mVariables[AI_VAR_SPEED] = StrToInt(data_str);
		}
	}
	else if(!strcmp(AI_VAR_FRAMES_STR, var_str))
	{
		if(!data_str)
		{
			function->mVariables[AI_VAR_FRAMES] = 1;
		} else
		{
			function->mVariables[AI_VAR_FRAMES] = StrToInt(data_str);
		}
	}
	else if(!strcmp(AI_VAR_TIME_STR, var_str))
	{
		if(!data_str)
		{
			function->mVariables[AI_VAR_TIME] = 1;
		} else
		{
			function->mVariables[AI_VAR_TIME] = StrToInt(data_str);
		}
	}
	else if(!strcmp(AI_VAR_DAMAGE_STR, var_str))
	{
		if(!data_str)
		{
			function->mVariables[AI_VAR_DAMAGE] = 1;
		} else
		{
			function->mVariables[AI_VAR_DAMAGE] = StrToInt(data_str);
		}
	}
}

/**
 * Sets AI action defined in ai_actions to the ai_function
 *
 * @param [in,out]	function  	If non-null, the function.
 * @param [in,out]	obj		  	If non-null, the object.
 * @param [in,out]	tok		  	If non-null, the tok.
 * @param [in,out]	g_str	  	If non-null, the string.
 * @param [in,out]	action_str	If non-null, the action string.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void SetAI_Action(ai_function_t* function, object_t* obj, jsmntok_t* tok, char* g_str, char* action_str)
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
			function->mVariables[AI_VAR_DIR_Y] = -3;
			
		} else if( (temp_vec2 = ParseToVec2(obj, g_str)) != NULL)
		{
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->x;
			function->mVariables[AI_VAR_DIR_X] = temp_vec2->y;
		} else
		{
			function->mVariables[AI_VAR_DIR_X] = 0;
			function->mVariables[AI_VAR_DIR_Y] = -3;
		}

		function->mAction = AI_ACTION_JUMP;
	}
	else if(!strcmp(AI_ACTION_ATTACK_STR, action_str))
	{
		function->mAction = AI_ACTION_ATTACK;
		function->mObject = JsmnToString(tok, g_str);
	}
}

/**
 * Sets checks that need to occur for the ai_function to be called.
 *
 * @param [in,out]	function	 	If non-null, the function.
 * @param [in,out]	variables_str	If non-null, the variables string.
 * @param [in,out]	data_str	 	If non-null, the data string.
 * @param [in,out]	check_str	 	If non-null, the check string.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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
		function->mVariables[AI_VAR_CHECK] = StrToInt(data_str);
	} 
	else if(!strcmp(AI_CHECK_PLAYER_STR, check_str))
	{
		function->mVariables[AI_VAR_CHECK] = StrToInt(data_str);
		function->mFlags |= AI_FLAG_CHECK_PLAYER;
	} 
	else if(!strcmp(AI_CHECK_OBJECT_STR, check_str))
	{
		function->mVariables[AI_VAR_CHECK] = StrToInt(data_str);
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

/**
 * Init AI system, similar to Entity init, for now....
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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

/**
 * Converts a str to an AI type.
 *
 * @param	str	The string.
 *
 * @return	str as an ai_type_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

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