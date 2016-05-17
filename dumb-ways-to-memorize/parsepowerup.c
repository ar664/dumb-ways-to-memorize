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
	for(i = 0; IterationSymbols[i]; i++)
	{
		if(!strcmp(str, IterationNames[i]))
		{
			return IterationSymbols[i];
		}
	}
	return NULL;
}

void IterateThroughWorld(power_t *power)
{

}

void IterateAtPoint(power_t *power)
{
	entity_t *temp_ent;
	vec2_t temp_pos;
	vec2_t *temp_vec = (vec2_t*) malloc(sizeof(vec2_t));
	if(!temp_vec)
	{
		return;
	}
	temp_ent = FindEntity("Cursor");
	temp_pos = EntityPosition(temp_ent);
	if(extra)
	{
		memcpy(extra, &temp_pos, sizeof(vec2_t));
	}
	*targ = LookForEntityAtPos(*temp_vec);
	if(temp_vec) free(temp_vec);
}

void IterateSelf(power_t *power)
{
	entity_t *temp_ent;
	vec2_t temp_pos;
	*targ = self;
	temp_ent = FindEntity("Cursor");
	temp_pos = EntityPosition(temp_ent);
	if(extra)
	{
		memcpy(extra, &temp_pos, sizeof(vec2_t));
	}
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
	power->IterateThroughTargets(power);
	power->uses--;
	if(power->uses == 0)
	{
		//free(power); Destroy Power
	}
}

void UpdateInfinite(power_t* power)
{
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
	retVal->name = obj->name;

	//Target Matching Function
	if( (temp_str = FindValue(obj, POWER_TARGET_STR, g_str)) != NULL )
	{
		retVal->DoPower = (PowerFunc) ParseToFunction(temp_str);
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
		retVal->IterateThroughTargets = (void(*)(power_t*)) ParseToIterator(temp_str);
		if(temp_str) free(temp_str);
	}
	
	//Assign Extra Values
	//Move Only Assigns Position
	if(retVal->DoPower == Move)
	{
		retVal->extra = malloc(sizeof(vec2_t));
	} else
	{
		if( (members = FindEntityMembers(obj, g_str)) != NULL)
		{
			retVal->extra = members;
		}
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

}
