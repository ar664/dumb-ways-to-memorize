#include <chipmunk/chipmunk.h>
#include "globals.h"
#include "ai_interpret.h"
#include "entity.h"
#include "player.h"
#include "mystrings.h"
#include "dumb_physics.h"
#include <stdio.h>
#include <stdlib.h>

//Forward Declare AI's to put into function list
void NothingAI(entity_t *ent); 
void MoveAI(entity_t *ent);
void WalkAI(entity_t *ent);
void JumpAI(entity_t *ent);
void AttackAI(entity_t *ent);

ai_function_t * gVariableAIs = NULL;
ai_function_t * gPresetAIs = NULL;

char *gAI_Variables[] = {"speed", "frames", "time", "damage", 0};
char *gAI_Actions[] = {"nothing", "move", "walk", "jump", "attack", 0};
char *gAI_Conditions[] = {"distance_player", "distance_object", "object_check", "link_ai", "link_action", 0};
void *gAI_Functions[] = {NothingAI, MoveAI, WalkAI, JumpAI, AttackAI, 0};
int gAI_BaseValues[] = {0, 5, 1, 1, 5, 0, 0, 0, 0};


//Run After All Think Functions
//This is very generic
void StandarAI_Think(entity_t *ent)
{
	entity_t *cache_ref;
	cpVect temp_cp;
	int flags;
	flags = ent->mData->mFlags;
	ent->mNextThink = gCurrentTime + ent->mData->mVariables[AI_VAR_FRAMES]*UPDATE_FRAME_DELAY;
	ent->mDamage = ent->mData->mVariables[AI_VAR_DAMAGE];
	cache_ref = FindCachedEntity(ent->mName);

	//Checks are right to left
	if(cache_ref->mPhysicsProperties && cache_ref)
	{
		if(flags & AI_FLAG_GRAVITY)
		{
			temp_cp.x = ent->mPhysicsProperties->body->v.x;
			temp_cp.y = 0;
			cpBodySetVel(ent->mPhysicsProperties->body, temp_cp);
		}
		
	}
	
	//Check Data
	if( --ent->mData->mVariables[AI_VAR_TIME] == 0)
	{
		ent->mData->mVariables[AI_VAR_TIME] = 1;
		ent->mData = ent->mData->mLink;
	}
	
	//Kill ent if dead
	if(ent->mHealth <= 0)
	{
		FreeEntity(ent);
	}
}

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
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}
	
	EntitySetAnimation(ent, ANIMATION_IDLE);

	//Standard Vars
	if(ent->mCollisionType != COLLISION_TYPE_RAGDOLL)
	{
		ent->mCollisionType = COLLISION_TYPE_RAGDOLL;
		SetCpCollisionType(ent);
	}

	StandarAI_Think(ent);
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
	vec2_t temp_vec2;
	cpVect cp_vect;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}

	//Standard Vars
	if(ent->mCollisionType != COLLISION_TYPE_CLIP)
	{
		ent->mCollisionType = COLLISION_TYPE_CLIP;
		SetCpCollisionType(ent);
	}
	
	//Set direction vector
	temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
	temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
	cp_vect = Vec2Cp(&temp_vec2);

	//Normalize & Mult speed
	cp_vect = cpvnormalize(cp_vect);
	cp_vect = cpvmult(cp_vect, ent->mData->mVariables[AI_VAR_SPEED]);

	//Set Velocity
	cpBodySetVel(ent->mPhysicsProperties->body, cp_vect);
	
	StandarAI_Think(ent);
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
	vec2_t temp_vec2;
	cpVect cp_vect;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}

	//Standard Vars
	if(ent->mCollisionType != COLLISION_TYPE_RAGDOLL)
	{
		ent->mCollisionType = COLLISION_TYPE_RAGDOLL;
		SetCpCollisionType(ent);
	}

	//Set direction vector
	temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
	temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
	cp_vect = Vec2Cp(&temp_vec2);

	//Normalize & Multiply by speed
	cp_vect = cpvnormalize(cp_vect);
	cp_vect = cpvmult(cp_vect, ent->mData->mVariables[AI_VAR_SPEED]);

	//Set Velocity
	cpBodySetVel(ent->mPhysicsProperties->body, cp_vect);

	StandarAI_Think(ent);
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
	vec2_t temp_vec2;
	cpVect cp_temp;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}

	//Standard Vars
	if(ent->mCollisionType != COLLISION_TYPE_RAGDOLL)
	{
		ent->mCollisionType = COLLISION_TYPE_RAGDOLL;
		SetCpCollisionType(ent);
	}

	//Move
	if(cpBodyKineticEnergy(ent->mPhysicsProperties->body) < 1 )
	{
		temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
		temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
		cp_temp = Vec2Cp(&temp_vec2);
		cp_temp = cpvnormalize(cp_temp);
		cp_temp = cpvmult(cp_temp, ent->mData->mVariables[AI_VAR_SPEED]);

		cpBodyApplyImpulse(ent->mPhysicsProperties->body, cp_temp, cpvzero);
		
	}

	StandarAI_Think(ent);
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
	entity_t *temp_ent;
	vec2_t temp_vec2;
	cpVect cp_temp;
	if(!ent->mData || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}
	cp_temp = cpvzero;

	//Standard Vars
	if(ent->mCollisionType != COLLISION_TYPE_RAGDOLL)
	{
		ent->mCollisionType = COLLISION_TYPE_RAGDOLL;
		SetCpCollisionType(ent);
	}
	
	if(ent->mData->mObject)
	{
		temp_ent = FindCachedEntity(ent->mData->mObject);
		if(temp_ent)
		{
			temp_vec2.x = ent->mData->mVariables[AI_VAR_DIR_X];
			temp_vec2.y = ent->mData->mVariables[AI_VAR_DIR_Y];
			cp_temp = Vec2Cp(&temp_vec2);
			AddPhyicsToEntity(temp_ent);
			cpBodySetVel(ent->mPhysicsProperties->body, cp_temp);
			
			Spawn(ent, &temp_ent, NULL);
		}
		
	}

	StandarAI_Think(ent);
}

void (*GetFunctionAI(ai_function_t *data))(entity_t *)
{
	if(!data)
	{
		return NULL;
	}
	if(data->mType > AI_ACTION_MAX)
	{
		return NULL;
	}
	return (void(*)(entity_t*))gAI_Functions[data->mType];
}

ai_function_t *ParseAI(object_t *obj, char *g_str, char **variables)
{
	int i, j, k,children, child_vars, gravity;
	int preset_flag, temp_x, temp_y, var_count;
	ai_function_t *retVal;
	jsmntok_t *action_tok;
	object_t *temp_obj, *action_obj, *variables_obj;
	char *temp_str, *temp_str2, *type_str, *cond_str,**variables_str;
	if(!obj || !g_str)
	{
		return NULL;
	}
	//If Variables present , the only supported flag is gravity
	gravity = FindObject(obj, AI_VAR_STR) ? 1 : 0;

	//Find the actual functions
	temp_obj = FindObject(obj, AI_FUNCTION_OBJECT);
	if(!temp_obj)
	{
		printf("No thinks in ai : %s \n", obj->name);
		return NULL;
	}
	
	//Find the type of AI it is
	type_str = FindValue(obj, AI_TYPE_STR, g_str);
	if(!type_str)
	{
		printf("No types in ai : %s \n", obj->name);
		return NULL;
	}

	children = CountMem(temp_obj->children, sizeof(object_t));
	retVal = (ai_function_t*) malloc(sizeof(ai_function_t)*(children+1));
	memset(retVal, 0, sizeof(ai_function_t)*(children+1));

	//Set preset flag based on if variables is null
	preset_flag = variables ? 0 : 1;
	var_count = 0;

	if(!preset_flag)
	{
		var_count = CountMem(variables, sizeof(char*));
	}

	for(i = 0; i < children; i++)
	{
		for(j = 0; gAI_Variables[j]; j++ )
		{
			temp_str = FindValue(&temp_obj->children[i], gAI_Variables[j], g_str);
			if(!temp_str)
			{
				SetAI_Var(&retVal[i], NULL, (ai_variables_t)j);
			} else
			{
				if(preset_flag)
				{
					SetAI_Var(&retVal[i], temp_str, (ai_variables_t)j );
				} else
				{
					temp_x = StrToInt(temp_str);
					if(temp_x > var_count)
					{
						printf("ERROR: AI Var %d not set. Variable above variable count. \n", j);
						SetAI_Var(&retVal[i], NULL, (ai_variables_t)j );
					} else
					{
						SetAI_Var(&retVal[i], variables[temp_x], (ai_variables_t)j );
					}
				}
			}
			
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		for(j = 0; gAI_Actions[j]; j++ )
		{	
			action_obj = FindObject(&temp_obj->children[i], gAI_Actions[j]);
			if(action_obj)
			{
				temp_str = JsmnToString(&action_obj->values[0], g_str);
				temp_str2 = JsmnToString(&action_obj->values[1], g_str);
				if(preset_flag)
				{
					SetAI_Action(&retVal[i], (ai_actions_t)j,  temp_str , temp_str2 );
				} else
				{
					temp_x = StrToInt(temp_str);
					temp_y = StrToInt(temp_str2);
					if(temp_x > var_count || temp_y > var_count)
					{
						printf("ERROR: AI Action %d not set. Variables above variable count. \n", j);
						SetAI_Action(&retVal[i], (ai_actions_t)j,  NULL , NULL );
					} else
					{
						SetAI_Action(&retVal[i], (ai_actions_t)j,  variables[temp_x] , variables[temp_y] );
					}
				}
				
				if(temp_str) free(temp_str);
				if(temp_str2) free(temp_str2);
				break;
			}
			action_tok = FindKey(temp_obj->children[i].keys, gAI_Actions[j], g_str);
			if(action_tok)
			{
				temp_str = JsmnToString(action_tok, g_str);
				if(preset_flag)
				{
					SetAI_Action(&retVal[i], (ai_actions_t)j,  temp_str , NULL );
				} else
				{

					temp_x = StrToInt(temp_str);
					if(temp_x > var_count)
					{
						printf("ERROR: AI Action %d not set. Variables above variable count. \n", j);
						SetAI_Action(&retVal[i], (ai_actions_t)j,  NULL , NULL );
					} else
					{
						SetAI_Action(&retVal[i], (ai_actions_t)j,  variables[temp_x] , NULL );
					}
				}
				if(temp_str) free(temp_str);
				break;
			}

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
				child_vars = CountObjectMembers(variables_obj, g_str);
				variables_str = (char**) malloc(sizeof(char*)*(child_vars+1));
				variables_str[child_vars] = NULL;
				for(k = 0; k < child_vars; k++)
				{
					temp_str = JsmnToString(&variables_obj->values[k], g_str);
					if(preset_flag)
					{
						variables_str[k] = temp_str ? temp_str : NULL;
					} else
					{
						temp_x = StrToInt(temp_str);
						if(temp_x > var_count)
						{
							printf("ERROR: AI Condition %d not set. Variables above variable count. \n", j);
							variables_str[k] = NULL;
						} else
						{
							variables_str[k] = variables[temp_x];
						}
					}
					
					if(temp_str) free(temp_str);
					temp_str = NULL;
				}
			}
			if(preset_flag)
			{
				SetAI_Check(&retVal[i], variables_str, cond_str, (ai_conditions_t)j );
			} else
			{
				temp_x = StrToInt(cond_str);
				if(temp_x > var_count)
				{
					printf("ERROR: AI Condition %d not set. Variables above variable count. \n", j);
					SetAI_Check(&retVal[i], variables_str, NULL, (ai_conditions_t)j );
				} else
				{
					SetAI_Check(&retVal[i], variables_str, variables[temp_x], (ai_conditions_t)j );
				}
			}

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

void SetAI_Var(ai_function_t* function, char* data_str, ai_variables_t var_type)
{
	if(!var_type || !function)
	{
		return;
	}

	function->mVariables[var_type] = data_str ? StrToInt(data_str) : gAI_BaseValues[var_type];
}

void SetAI_Action(ai_function_t* function, ai_actions_t action_type, char * x, char * y)
{
	if(!action_type || !function)
	{
		return;
	}
	switch(action_type)
	{
	case AI_ACTION_ATTACK:
		{
			function->mAction = AI_ACTION_ATTACK;
			function->mObject = x;
			break;
		}
	case AI_ACTION_JUMP:
		{
			function->mVariables[AI_VAR_DIR_X] = x ? StrToInt(x) : gAI_BaseValues[AI_VAR_DIR_X];
			function->mVariables[AI_VAR_DIR_Y] = y ? StrToInt(y) : gAI_BaseValues[AI_VAR_DIR_Y] - 5;
			function->mAction = action_type;
			break;
		}
	default:
		{
			function->mVariables[AI_VAR_DIR_X] = x ? StrToInt(x) : gAI_BaseValues[AI_VAR_DIR_X];
			function->mVariables[AI_VAR_DIR_Y] = y ? StrToInt(y) : gAI_BaseValues[AI_VAR_DIR_Y];
			function->mAction = action_type;
		}
		break;
	}
}

void SetAI_Check(ai_function_t* function, char** variables_str, char* data_str, ai_conditions_t condition)
{
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
			if(temp_str) free(temp_str);
			if(temp_tok) free(temp_tok);
			if(temp_obj) FreeObject(temp_obj);
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
