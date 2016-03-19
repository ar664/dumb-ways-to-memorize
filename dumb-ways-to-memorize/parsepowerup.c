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
	targ->mPosition = info->mPosition;
}


//No other access necessary
void Destroy(entity_t *targ, entity_t *info)
{
	if(!targ)
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
		printf("Spawn given blank targ/info");
		return;
	}
	spawned = InitNewEntity();
	if (!spawned)
	{
		printf("Max entities reached can't spawn any more");
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
		printf("Null edit, not doing");
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
	targ->Think = NULL;
}

void UpdateNormal(power_t* power)
{
	if(!Player)
	{
		printf("Player not set");
		return;
	}
	if(!power)
	{
		printf("power not set");
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
		sscanf(var, "%d", *useType);
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

power_t* ParseToPowerUp(object_t* power, char* str)
{
	int i;
	power_t *retVal;
	jsmntok_t *temp;
	retVal = (power_t*) malloc(sizeof(power_t));
	if(!retVal)
	{
		printf("Power up malloc error \n");
		return NULL;
	}
	if( (temp = FindKey(power->keys, "name", str)) != NULL )
	{
		retVal->name = FindValueFromKey(temp, "name", str);
	}
	if( (temp = FindKey(power->keys, "target", str)) != NULL )
	{
		retVal->GetTarg = ParseToFunction(FindValueFromKey(temp, "target", str));
	}
	//Use Type
	if( (temp = FindKey(power->keys, "use-type", str)) != NULL )
	{
		GetUseType(FindValueFromKey(temp, "use-type", str), &retVal->uses);
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
	if( (temp = FindKey(power->keys, "input-type", str)) != NULL )
	{
		for(i = 0; i < INFO_BOTH; i++ )
		{
			if(strcmp(InteractionNames[i], FindValueFromKey(temp, "input-type", str)))
			{
				retVal->info_type = (info_type_t) (INFO_BOTH - i);
				break;
			}
			retVal->info_type = INFO_NONE;
		}
	}
	if(retVal->info_type)
	{
		retVal->UpdateInput = CallInfo;
	}

	//Interaction
	if( (temp = FindKey(power->keys, "target", str)) != NULL )
	{
		for(i = 0; *FunctionNames[i]; i++ )
		{
			if(strcmp(FunctionNames[i], FindValueFromKey(temp, "target", str)))
			{
				retVal->DoPower = (void(*)(entity_t *targ, entity_t *info)) InteractionSymbols[i];
				break;
			}
			retVal->DoPower = NULL;
		}
	}
	retVal->info = ParseToEntity(power, str);


	return retVal;
}

void UsePower(power_t* power)
{
	power->UpdateInput(power);
	power->UpdateUse(power);
	power->DoPower(power->target, power->info);
}
