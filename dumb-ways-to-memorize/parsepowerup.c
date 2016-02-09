#include "globals.h"
#include "parsefunction.h"
#include "parsepowerup.h"
#include "parseobject.h"
#include "entity.h"
#include "mystrings.h"
#include <jsmn.h>
#include <string.h>

char *InteractionNames[] = {"move", "destroy", "spawn", "edit", "nullify", 0};
void (*InteractionSymbols[]) =  {Move, Destroy, Spawn, Edit, Nullify, 0};

vec2_t mousePos  = {-1,-1};
int keyPower = 0;

//Needs math and time
void Move()
{
	//Targ->Move to Pos
	//targEnt->mVelocity = targPos; //TODO math
}


//No other access necessary
void Destroy()
{
	//freeEnt(Targ)
}

//Needs access to parseEntity
void Spawn(object_t *object, char *g_str)
{
	//Targ->Spawn
	
}

//Needs more information
void Edit(void *args, ...)
{
	//Targ->Edit
}

//No other access neccesary
void Nullify()
{
	//Targ->Think = NULL;
	//targEnt->Think = NULL;
}


void UpdateNormal(power_t* power)
{
	power->GetTarg((entity_t*) Player, &power->target);
	power->uses--;
	if(power->uses == 0)
	{
		free(power);
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

void CallInfo(info_type_t info)
{
	switch (info)
	{
	case INFO_BOTH:
		GetXMouse((entity_t*) Player, &keyPower, &mousePos); break;
	case INFO_BUTTON:
		GetX((entity_t*) Player, &keyPower); break;
	case INFO_MOUSE:
		GetMousePos((entity_t*) Player, &mousePos); break;
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
	if( (temp = FindKey(power->keys, "name", str)) != NULL )
	{
		retVal->name = FindValueFromKey(temp, "name", str);
	}
	if( (temp = FindKey(power->keys, "target", str)) != NULL )
	{
		retVal->GetTarg = ParseToFunction(FindValueFromKey(temp, "target", str));
	}
	if( (temp = FindKey(power->keys, "use-type", str)) != NULL )
	{
		GetUseType(FindValueFromKey(temp, "use-type", str), &retVal->uses);
	}
	switch(retVal->uses)
	{
	case INFINITE:
		retVal->Update = UpdateInfinite;
		break;
	case STATIC:
		retVal->Update = NULL;
		break;
	default:
		retVal->Update = UpdateNormal;
	}

	if( (temp = FindKey(power->keys, "interaction", str)) != NULL )
	{
		for(i = 0; FunctionNames[i]; i++ )
		{
			if(strcmp(FunctionNames[i], FindValueFromKey(temp, "interaction", str)))
			{
				retVal->info_type = (info_type_t) (3 - (i)%3); //Horrible macro
			}
		}
	}
	if(retVal->info_type)
	{
		retVal->GetInfo = CallInfo;
	}


	return retVal;
}

