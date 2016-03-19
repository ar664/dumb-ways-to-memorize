#include "globals.h"
#include "parsefunction.h"
#include "parsepowerup.h"
#include "parseobject.h"
#include "entity.h"
#include "mystrings.h"
#include <jsmn.h>
#include <string.h>
#include <stdio.h>
#include "parsevariable.h"

char *InteractionNames[] = {"move", "destroy", "spawn", "edit", "nullify", 0};
void (*InteractionSymbols[]) =  {Move, Destroy, Spawn, Edit, Nullify, 0};

vec2_t *mousePos  = NULL;
int *keyPower = NULL;

void Move(entity_t *targ, entity_t *info)
{
	if(!targ || !info)
	{
		printf("Failed to do move , invalid target/info \n");
		return;
	}
	targ->mPosition = info->mPosition;
}


//No other access necessary
void Destroy(entity_t *targ, entity_t *info)
{
	if(targ)
	{
		FreeEntity(targ);
	}
}

//Needs access to parseEntity
void Spawn(entity_t *targ, entity_t *info)
{
	entity_t *spawned;
	if(!targ || !info)
	{
		printf("Spawn given blank targ/info \n");
		return;
	}
	spawned = InitNewEntity();
	if (!spawned)
	{
		printf("Max entities reached can't spawn any more \n");
		return;
	}
	spawned->mHazards = info->mHazards;
	spawned->mCollisionType = info->mCollisionType;
	spawned->mEntityState = info->mEntityState;
	spawned->mSprites = info->mSprites;
	spawned->mName = info->mName;
	spawned->mAccel = info->mAccel;
	spawned->mVelocity = info->mVelocity;
	Vec2Add(&targ->mPosition, &info->mPosition, &spawned->mPosition);
}

void Edit(entity_t *targ, entity_t *info)
{
	int i, entity_size;
	int *dst = (int*) targ;
	int *value = (int*) info;
	if(!targ || !info)
	{
		printf("Null edit, not doing \n");
		return;
	}
	entity_size = sizeof(entity_t)/sizeof(int);
	//iterate through members
	for(i = 0; i < entity_size; i++)
	{
		if(*&value[i] && (*&dst[i] != *&value[i]))
		{
			*&dst[i] = *&value[i];
		}
	}
	
}

void Nullify(entity_t *targ, entity_t *info)
{
	if(!targ)
	{
		printf("Null given null target \n");
	}
	targ->Think = NULL;
}

void UpdateNormal(power_t* power)
{
	if(!Player)
	{
		printf("Player not set \n");
		return;
	}
	if(!power)
	{
		printf("power not set \n");
		return;
	}
	power->GetTarg((entity_t*) Player, &power->target);
	power->uses--;
	if(power->uses == 0)
	{
		//free(power); Destroy Power
	}
}

void UpdateInfinite(power_t* power)
{
	power->GetTarg( (entity_t*) Player, &power->target);
}

int GetUseType(const char *var, int *useType)
{
	if(!strcmp("infinite", var))
	{
		*useType = -1;
		return 0;
	} else if(!strcmp("static", var))
	{
		*useType = 0;
		return 0;
	} else if(strpbrk(var, "-0123456789"))
	{
		sscanf(var, "%d", useType);
		return 0;
	}
	return -1;
}

void CallInfo(power_t *self)
{
	switch (self->info_type)
	{
	case INFO_BOTH:
		GetXMouse((entity_t*) Player, keyPower, mousePos); break;
	case INFO_BUTTON:
		GetX((entity_t*) Player, keyPower); break;
	case INFO_MOUSE:
		GetMousePos((entity_t*) Player, mousePos); break;
	default:
		break;
	}
}

power_t* ParseToPowerUp(object_t* power, char* g_str)
{
	int i;
	char *temp_str;
	jsmntok_t *temp_tok;
	entity_t *temp_ent;
	power_t *retVal;
	retVal = (power_t*) malloc(sizeof(power_t));
	if(!retVal)
	{
		printf("Power up malloc error \n");
		return NULL;
	}
	if( (temp_tok = FindKey(power->keys, "name", g_str)) != NULL )
	{
		temp_str = JsmnToString(&power->values[temp_tok-power->keys], g_str);
		retVal->name = temp_str;
	}
	if( (temp_tok = FindKey(power->keys, "target", g_str)) != NULL )
	{
		temp_str = JsmnToString(&power->values[temp_tok-power->keys], g_str);
		retVal->GetTarg = ParseToFunction(temp_str);
		if(temp_str) free(temp_str);
	}
	//Use Type
	if( (temp_tok = FindKey(power->keys, "use-type", g_str)) != NULL )
	{
		temp_str = JsmnToString(&power->values[temp_tok-power->keys], g_str);
		GetUseType(temp_str, &retVal->uses);
		if(temp_str) free(temp_str);
	}
	switch(retVal->uses)
	{
	case INFINITE:
		retVal->UpdateUse = UpdateInfinite;
		break;
	case STATIC:
		retVal->UpdateUse = NULL;
		break;
	default:
		retVal->UpdateUse = UpdateNormal;
	}

	//Input Type
	if( (temp_tok = FindKey(power->keys, "input-type", g_str)) != NULL )
	{
		for(i = 0; i < INFO_BOTH; i++ )
		{
			temp_str = JsmnToString(&power->values[temp_tok-power->keys], g_str);
			if(!strcmp(FunctionNames[i], temp_str))
			{
				retVal->info_type = (info_type_t) (INFO_BOTH - i);
				break;
			}
			retVal->info_type = INFO_NONE;
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		if(temp_str) free(temp_str);
	}
	if(retVal->info_type)
	{
		retVal->UpdateInput = CallInfo;
		keyPower = (int*) malloc(sizeof(int));
		memset(keyPower,0, sizeof(int));
		mousePos = (vec2_t*) malloc(sizeof(vec2_t));
		memset(mousePos,0, sizeof(vec2_t));
	}

	//Interaction
	if( (temp_tok = FindKey(power->keys, "target", g_str)) != NULL )
	{
		for(i = 0; *FunctionNames[i]; i++ )
		{
			temp_str = JsmnToString(&power->values[temp_tok-power->keys], g_str);
			if(!strcmp(FunctionNames[i], temp_str))
			{
				retVal->DoPower = (void(*)(entity_t *targ, entity_t *info)) InteractionSymbols[i];
				break;
			}
			retVal->DoPower = NULL;
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
		if(temp_str) free(temp_str);
	}
	
	//Info
	if( (temp_tok = FindKey(power->keys, "entity", g_str)) != NULL )
	{
		temp_str = JsmnToString(&power->values[temp_tok-power->keys], g_str);
		if( (temp_ent = FindCachedEntity( temp_str )) != NULL )
		{
			retVal->info = ParseToEntity(power, g_str);
		} else
		{
			printf("Failed to identify/find entity in power : %s \n", power->name);
			retVal->info = NULL;
		}
		if(temp_str) free(temp_str);
	} else
	{
		retVal->info = ParseToEntity(power, g_str);
	}

	return retVal;
}

void UsePower(power_t* power)
{
	power->UpdateInput(power);
	power->UpdateUse(power);
	power->DoPower(power->target, power->info);
}
