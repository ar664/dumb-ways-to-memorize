 #include "globals.h"
#include "parsefunction.h"
#include "parsepowerup.h"
#include "parseobject.h"
#include "parseentity.h"
#include "entity.h"
#include "mystrings.h"
#include <jsmn.h>
#include <string.h>
#include <stdio.h>
#include "dumb_physics.h"
#include "player.h"

char *IterationNames[] = {"self", "at-point", "world" , 0};
void (*IterationSymbols[]) =  {IterateSelf, IterateAtPoint, IterateThroughWorld, 0 };
void (*LocationSymbols[]) = {GetSelf, GetPoint, GetPoint, 0};

power_t *gCurrentPowerUp = NULL;
vec2_t *mousePos  = NULL;
int *keyPower = NULL;


void (*ParseToIterator( char *str))
{
	int i;
	if(!str)
	{
		return NULL;
	}
	for(i = 0; IterationNames[i]; i++)
	{
		if(!strcmp(str, IterationNames[i]))
		{
			return IterationSymbols[i];
		}
	}
	return NULL;
}

//////////////
//Iterators 

void IterateThroughWorld(power_t *power)
{
	int i;
	if(!power)
	{
		return;
	}
	if(!power->DoPower)
	{
		return;
	}

	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].Think)
		{
			continue;
		}
		if(&gEntities[i] == gPlayer || &gEntities[i] == gCursor)
		{
			continue;
		}
		if(power->DoPower == Move || power->DoPower == Spawn)
		{
			power->DoPower(&gEntities[i], &power->info, power->location);
		} else
		{
			power->DoPower(&gEntities[i], &power->info, power->members);
		}
	}
}

void IterateAtPoint(power_t *power)
{
	entity_t *temp_ent;
	vec2_t temp_pos;
	if(!power)
	{
		return;
	}
	if(!power->location || !power->DoPower)
	{
		return;
	}
	temp_ent = FindEntity("Cursor");
	temp_pos = EntityPosition(temp_ent);
	//TODO LookForEntity Doesn't hit Cursor
	power->target = LookForEntityAtPos(temp_pos);
	if(power->DoPower == Move || power->DoPower == Spawn)
	{
		power->DoPower(power->target, &power->info, power->location);
	} else
	{
		power->DoPower(power->target, &power->info, power->members);
	}

}

void IterateSelf(power_t *power)
{
	vec2_t *loc;
	if(!power)
	{
		return;
	}
	if(!power->DoPower)
	{
		return;
	}
	power->target = gPlayer;

	if(power->DoPower == Move || power->DoPower == Spawn)
	{
		power->DoPower(power->target, &power->info, power->location);
	} else
	{
		power->DoPower(power->target, &power->info, power->members);
	}
}

LocationFunc ParseToLocation(char* str)
{
	int i;
	if(!str)
	{
		return NULL;
	}
	for(i = 0; IterationNames[i]; i++)
	{
		if(!strcmp(str, IterationNames[i]))
		{
			return (LocationFunc) LocationSymbols[i];
		}
	}
	return NULL;
}

//////////////////////
//Location Functions

vec2_t GetSelf(entity_t *ent)
{
	vec2_t temp_vect;
	if(!ent)
	{
		return gZeroVect;
	}

	if(!gPlayer)
	{
		return gZeroVect;
	}

	if(!gPlayer->mPhysicsProperties)
	{
		return gZeroVect;
	}

	temp_vect = CpToVect(&gPlayer->mPhysicsProperties->body->p);
	return temp_vect;
}

vec2_t GetPoint(entity_t *ent)
{
	entity_t *temp_ent;
	vec2_t temp_vect;
	if(!ent)
	{
		return gZeroVect;
	}

	if(!ent->mPhysicsProperties)
	{
		return gZeroVect;
	}

	temp_ent = FindEntity("Cursor");

	if(!temp_ent)
	{
		return gZeroVect;
	}

	temp_vect = EntityPosition(temp_ent);
	return temp_vect;
}

////////////////////////
//Update Functions

void UpdateNormal(power_t* power)
{
	if(!gPlayer)
	{
		printf("Player not set \n");
		return;
	}
	if(!power)
	{
		printf("Power not set \n");
		return;
	}
	power->IterateThroughTargets(power);
	power->uses--;
	if(power->uses == 0)
	{
		//power->UpdateUse = NULL;
	}
}

void UpdateInfinite(power_t* power)
{
	if(!gPlayer)
	{
		return;
	}
	if(!power)
	{
		return;
	}
	power->IterateThroughTargets( power );
}

int GetUseType(power_t *power, const char *var)
{
	if(!strcmp("infinite", var))
	{
		power->uses = -2;
		power->UpdateUse = UpdateInfinite;
		return 0;
	} 
	if(!strcmp("static", var))
	{
		power->uses = -1;
		return 0;
	} 
	if(strpbrk(var, "-0123456789"))
	{
		sscanf(var, "%d", &power->uses);
		power->UpdateUse = UpdateNormal;
		return 0;
	}
	return -1;
}

/////////////////
// Actual Parsing

power_t* ParseToPowerUp(object_t* obj, char* g_str)
{
	int i;
	char *temp_str;
	entity_t *temp_ent;
	entity_member_t *members;
	power_t *retVal;
	
	if(!obj || !g_str)
	{
		printf("Power Up tried to parse NULL \n");
		return NULL;
	}
	if (!obj->name)
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
	retVal->name = strdup(obj->name);

	//Target Matching Function
	if( (temp_str = FindValue(obj, POWER_TARGET_STR, g_str)) != NULL )
	{
		retVal->IterateThroughTargets = (void(*)(power_t*)) ParseToIterator(temp_str);
		if(temp_str) free(temp_str);
		
	}

	//Get How the power up is used
	if( (temp_str = FindValue(obj, POWER_USE_TYPE_STR, g_str)) != NULL )
	{
		GetUseType(retVal, temp_str);
		if(temp_str) free(temp_str);
	}

	//Interaction
	if( (temp_str = FindValue(obj, POWER_INTERACTION_STR, g_str)) != NULL )
	{
		retVal->DoPower = (PowerFunc) ParseToFunction(temp_str);
		if(temp_str) free(temp_str);
	}

	//Assign Location Value (If any)
	if( (temp_str = FindValue(obj, POWER_PLACE_STR, g_str)) != NULL )
	{
		retVal->location = ParseToLocation(temp_str);
		if(temp_str) free(temp_str);
	}

	//Assign Member Value(s) (If any)
	if( (members = FindEntityMembers(obj, g_str)) != NULL)
	{
		retVal->members = members;
	}

	//The target's information if needed
	//For use with Spawn
	if( (temp_str = FindValue(obj, POWER_ENTITY_STR, g_str)) != NULL )
	{
		if( (temp_ent = FindCachedEntity( temp_str )) != NULL )
		{
			retVal->info = temp_ent;
		} else
		{
			printf("Failed to identify/find entity in power : %s \n", obj->name);
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
	
}

void FreePower(power_t *power)
{
	if(!power)
	{
		return;
	}
	if(power->members)
	{
		FreeEntityMembers((entity_member_t*)power->members);
		power->members = NULL;
	}
	if(power->location)
	{
		free(power->location);
		power->location = NULL;
	}
	if(power->icon)
	{
		if(gGameState == END)
		{
			FreeSprite(power->icon);
		}
		power->icon = NULL;
	}
	
	//free(power);

}
