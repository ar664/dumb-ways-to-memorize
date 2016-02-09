#include "parsevariable.h"
#include "parseobject.h"
#include "mystrings.h"
#include <jsmn.h>
#include <string.h>
#include <stdio.h>

//Globals
char *AssignableVariableNames[] = {"hazard(s)" , "collisionType", "entityState", "sprite(s)", "accel", "velocity", "position", 0};
char *OtherVariableNames[] = { "height", "width", "frames", 0 };

//BIG ASS SWITCH CASE
void EditEntity(entity_t  *ent, EntityNumbers member, void *value)
{
	switch(member)
	{
	case HAZARDS: ent->mHazards = *((int*) value); break;
	case COLLISION_TYPE: ent->mCollisionType = *((collision_type_t*) value); break;
	case ENTITY_STATE: ent->mEntityState = *((entity_state_t*) value); break;
	case SPRITES: ent->mSprites = (sprite_t*) value; break;
	case ACCEL: ent->mAccel = *((vec2_t*) value); break;
	case VELOCITY : ent->mVelocity = *((vec2_t*) value); break;
	case POSITION : ent->mPosition = *((vec2_t*) value); break;
	default:
		break;
	}
}

entity_t* ParseToEntity(object_t* object, char* str)
{
	int i, j, checkInt;
	entity_t *retVal;
	jsmntok_t *checkTok;
	object_t *checkObj;
	retVal = (entity_t*) malloc(sizeof(entity_t));
	for(i = 0; AssignableVariableNames[i]; i++)
	{
		if( (checkTok = FindKey(object->keys, AssignableVariableNames[i], str)) != NULL)
		{
			checkObj = FindObject(object, AssignableVariableNames[i]);
			if(CountMem(checkObj->values, sizeof(jsmntok_t)) == 2 && CountMem(checkObj->keys, sizeof(jsmntok_t)) == 0)
			{
				EditEntity(retVal, (EntityNumbers)i, ParseToVec2(checkObj, str));
			} else if(CountMem(checkObj->values, sizeof(jsmntok_t)) > 2)
			{
				if(i == HAZARDS)
				{
					for(j = 0; j < checkTok->size; j++)
						checkInt = j;
				} else
				{
					for(j = 0; j < checkTok->size; j++)
						checkInt = 0;
				}
			} else if(checkTok->type == JSMN_PRIMITIVE || checkTok->type == JSMN_STRING)
			{
				
			}
		}
	}
	return NULL;
}

vec2_t* ParseToVec2(object_t* object, char* str)
{
	return NULL;
}

char **ParseToStringArray(object_t* object, char* str)
{
	int i, size;
	char *temp;
	char **retVal;
	size = CountMem(object->values, sizeof(jsmntok_t));
	retVal = (char**) malloc(sizeof(char*)*(size+1));
	for(i = 0; i < size; i++)
	{
		temp = JsmnToString(&object->values[i], str);
		retVal[i] = temp;
	}
	return retVal;
}
