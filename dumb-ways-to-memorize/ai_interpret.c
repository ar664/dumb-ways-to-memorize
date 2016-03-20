#include "ai_interpret.h"
#include "entity.h"
#include "mystrings.h"
#include <stdio.h>

ai_function_t * gVariableAIs = NULL;
ai_function_t * gPresetAIs = NULL;

void MoveAI(entity_t *ent)
{
	KV_Pair_t *temp_kv;
	int *temp_i;
	vec2_t *temp_vec2;
	if(!ent->data || !ent)
	{
		printf("MoveAI given a null paramerter \n");
		return;
	}
	if(ent->data->mFlags)
	{
		if(ent->data->mFlags & AI_FLAG_FRAMES)
		{
			temp_kv = FindKVPair(ent->data->mVariables,AI_VAR_FRAMES_STR);
			temp_i = (int*)temp_kv->value;
			if(temp_kv)
			{
				ent->mNextThink = SDL_GetTicks()+*temp_i;
			}
		} 

		if(ent->data->mFlags & AI_FLAG_DAMAGE)
		{
			temp_kv = FindKVPair(ent->data->mVariables,AI_VAR_FRAMES_STR);
			temp_i = (int*)temp_kv->value;
			if(temp_kv)
			{
				ent->mDamage = *temp_i;
			}
		}

		if(ent->data->mFlags & AI_FLAG_GRAVITY)
		{
			ent->mWeight = 0;
		} else if(!ent->mWeight)
		{
			ent->mWeight = FindCachedEntity(ent->mName)->mWeight;
		}

		if(ent->data->mFlags & AI_FLAG_VELOCITY)
		{
			temp_kv = FindKVPair(ent->data->mVariables,AI_VAR_VELOCITY_STR);
			temp_vec2 = (vec2_t*)temp_kv->value;
			if(temp_kv)
			{
				Vec2Add(&ent->mVelocity,temp_vec2, &ent->mVelocity);
			}
		}

		if(ent->data->mFlags & AI_FLAG_TIME)
		{
			if(ent->data->mTime)
			{
				ent->Think = MoveAI;
				ent->data->mTime--;
				if(ent->data->mTime < 0)
				{
					ent->data->mTime = 0;
				}
			} else if(ent->data->mLink)
			{
				ent->data = ent->data->mLink;
			}
		}
	}
}


ai_function_t* ParseAI(object_t* obj, char* g_str)
{
	int i;
	ai_actions_t *retVal;
	char *temp_str;

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
