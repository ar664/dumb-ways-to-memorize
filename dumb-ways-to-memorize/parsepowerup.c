#include "globals.h"
#include "parsefunction.h"
#include "parsepowerup.h"
#include "parseobject.h"
#include "entity.h"
#include "mystrings.h"
#include <jsmn.h>
#include <string.h>
#include <stdio.h>
#include "dumb_physics.h"
#include "player.h"

char *InteractionNames[] = {"move", "destroy", "spawn", "edit", "nullify", 0};
void (*InteractionSymbols[]) =  {Move, Destroy, Spawn, Edit, Nullify, 0};

power_t *gCurrentPowerUp = NULL;
vec2_t *mousePos  = NULL;
int *keyPower = NULL;

void Move(entity_t *targ, entity_t **info, void *extra)
{
	cpVect *cpPos;
	if(!targ || !extra)
	{
		printf("Failed to do move , invalid target/info \n");
		return;
	}
	if(!targ->mPhysicsProperties)
	{
		printf("Failed to do move , no target/info physics properties \n");
		return;
	}
	cpPos = (cpVect*)Vec2Cp((vec2_t*)extra);
	if(cpPos)
	{
		cpBodySetPos(targ->mPhysicsProperties->body, *cpPos);
		free(cpPos);
	}
	

}


//No other access necessary
void Destroy(entity_t *targ, entity_t **info, void *extra)
{
	int world = 0;
	if(extra)
	{
		world = *( (int*) extra);
	}
	if(targ && world)
	{
		FreeEnemyEntities();
	} else
	{
		FreeEntity(targ);
	}
}

//Needs access to parseEntity
void Spawn(entity_t *targ, entity_t **info, void *extra)
{
	entity_t *spawned;
	cpVect cpPos, cpSpeed;
	if(!targ || !info)
	{
		printf("Spawn given blank targ/info \n");
		return;
	}
	if(!targ->mPhysicsProperties || !(*info)->mPhysicsProperties)
	{
		printf("Spawn given info without physics \n");
		return;
	}

	spawned = InitNewEntity();
	if (!spawned)
	{
		printf("Max entities reached can't spawn any more \n");
		return;
	}

	memcpy(spawned, *info, sizeof(entity_t));
	AddPhyicsToEntity(spawned);
	if(!spawned->mPhysicsProperties)
	{
		printf("Unable to spawn entity, physics could not be added. \n");
		return;
	}
	spawned->mPhysicsProperties->body->p = (*info)->mPhysicsProperties->body->p;
	cpPos = cpvadd(cpBodyGetPos(spawned->mPhysicsProperties->body), cpBodyGetPos(targ->mPhysicsProperties->body));

	if(targ->mDirection == DIR_RIGHT)
	{
		cpPos.x += targ->mSprites[ANIMATION_IDLE]->mSize.x + 10;
		cpBodySetPos(spawned->mPhysicsProperties->body, cpPos);
		cpSpeed.y = 0;
		cpSpeed.x = PHYSICS_BASE_SPEED_X;
		cpBodyApplyForce( spawned->mPhysicsProperties->body, cpSpeed, cpvzero);
	} else
	{
		cpPos.x -= targ->mSprites[ANIMATION_IDLE]->mSize.x + 10;
		cpBodySetPos(spawned->mPhysicsProperties->body, cpPos);
		cpSpeed.y = 0;
		cpSpeed.x = -PHYSICS_BASE_SPEED_X;
		cpBodyApplyForce( spawned->mPhysicsProperties->body, cpSpeed, cpvzero);
	}
	AddEntityToPhysics(spawned);
}

void Edit(entity_t *targ, entity_t **info, void *extra)
{
	int i, entity_size;
	int *dst = (int*) targ;
	int *value = (int*) *info;
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

void Nullify(entity_t *targ, entity_t **info, void *extra)
{
	if(!targ)
	{
		printf("Null given null target \n");
	}
	targ->Think = NULL;
}

void* GetExtraMem(int interaction)
{
	void *mem;
	switch(interaction)
	{
	case 0:
		{
			mem = malloc(sizeof(vec2_t));
			if(!mem)
			{
				printf("ERROR: Allocation of memory for power up gone wrong. \n");
			}
			memset(mem, 0, sizeof(vec2_t));
			return mem;
			break;
		}
	case 2:
		{
			break;
		}
	default:
		{
			break;
		}
	}
	return NULL;
}

void UpdateNormal(power_t* power)
{
	if(!gPlayer)
	{
		printf("Player not set \n");
		return;
	}
	if(!power)
	{
		printf("power not set \n");
		return;
	}
	power->GetTarg((entity_t*) gPlayer, &power->target, power->extra);
	power->uses--;
	if(power->uses == 0)
	{
		//free(power); Destroy Power
	}
}

void UpdateInfinite(power_t* power)
{
	power->GetTarg( (entity_t*) gPlayer, &power->target, power->extra);
}

int GetUseType(power_t *power, const char *var)
{
	if(!strcmp("infinite", var))
	{
		power->uses = -2;
		power->UpdateUse = UpdateInfinite;
		return 0;
	} else if(!strcmp("static", var))
	{
		power->uses = -1;
		return 0;
	} else if(strpbrk(var, "-0123456789"))
	{
		sscanf(var, "%d", &power->uses);
		power->UpdateUse = UpdateNormal;
		return 0;
	}
	return -1;
}

power_t* ParseToPowerUp(object_t* power_obj, char* g_str)
{
	int i;
	char *temp_str;
	entity_t *temp_ent;
	power_t *retVal;
	
	if(!power_obj || !g_str)
	{
		printf("Power Up tried to parse NULL \n");
		return NULL;
	}
	if (!power_obj->name)
	{
		printf("Tried to parse Power Up with No Name");
		return NULL;
	}

	//Allocate for powerup
	retVal = (power_t*) malloc(sizeof(power_t));
	if(!retVal)
	{
		printf("Power up malloc error \n");
		return NULL;
	}
	memset(retVal, 0, sizeof(power_t));
	retVal->name = power_obj->name;

	//Target Matching Function
	if( (temp_str = FindValue(power_obj, POWER_TARGET_STR, g_str)) != NULL )
	{
		retVal->GetTarg = (CustomFunc) ParseToFunction(temp_str);
		if(temp_str) free(temp_str);
	}

	//Get How the power up is used
	if( (temp_str = FindValue(power_obj, POWER_USE_TYPE_STR, g_str)) != NULL )
	{
		GetUseType(retVal, temp_str);
		if(temp_str) free(temp_str);
	}

	//Interaction
	if( (temp_str = FindValue(power_obj, POWER_INTERACTION_STR, g_str)) != NULL )
	{
		for(i = 0; InteractionNames[i]; i++ )
		{
			if(!strcmp(InteractionNames[i], temp_str))
			{
				retVal->DoPower = (CustomFunc)InteractionSymbols[i];
				retVal->extra = GetExtraMem(i);
				break;
			}
			retVal->DoPower = NULL;
		}
		if(temp_str) free(temp_str);
	}
	
	//The target's information if needed
	if( (temp_str = FindValue(power_obj, POWER_ENTITY_STR, g_str)) != NULL )
	{
		if( (temp_ent = FindCachedEntity( temp_str )) != NULL )
		{
			retVal->info = temp_ent;
		} else
		{
			printf("Failed to identify/find entity in power : %s \n", power_obj->name);
			retVal->info = NULL;
		}
		if(temp_str) free(temp_str);
	} else
	{
		retVal->info = NULL;//ParseToEntity(power, g_str);
	}

	return retVal;
}

power_t* FindPower(char* str)
{
	int i, count;
	if(!gPowerUps || !str)
	{
		printf("ERROR: No PowerUps Loaded \n");
		return NULL;
	}
	count = CountMem(gPowerUps, sizeof(power_t));
	for(i = 0; i < count; i++)
	{
		if(!gPowerUps[i].name)
			continue;
		if(!strcmp(gPowerUps[i].name, str))
		{
			return &gPowerUps[i];
		}
	}
	return NULL;
}

void UsePower(power_t* power)
{
	if(!power)
	{
		return;
	}
	if(power->UpdateUse)
	{
		power->UpdateUse(power);
	}
	if(power->DoPower)
	{
		power->DoPower(power->target, &power->info, power->extra);
	}
	
}
