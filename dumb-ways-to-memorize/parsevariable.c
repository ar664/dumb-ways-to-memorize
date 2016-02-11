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
	int i, j, checkInt = 0;
	entity_t *retVal;
	sprite_t *checkSprite = NULL;
	jsmntok_t *checkTok = NULL;
	object_t *checkObj = NULL;
	retVal = (entity_t*) malloc(sizeof(entity_t));
	for(i = 0; AssignableVariableNames[i]; i++)
	{
		if( (checkTok = FindKey(object->keys, AssignableVariableNames[i], str)) != NULL)
		{
			if( (checkObj = FindObject(object, AssignableVariableNames[i])) != NULL)
			{
				if(i == HAZARDS)
				{
					for(j = 0; j < checkTok->size; j++)
						checkInt += StrToHazard(JsmnToString(&checkObj->values[i+j], str));
					EditEntity(retVal, (EntityNumbers)i, (void*)StrToHazard(Hazards_str[i]));
				} else if(i == SPRITES)
				{
					for(j = 0; j < checkTok->size; j++)
					{
						AllocateDynamic((void**)&checkSprite, LoadSprite(JsmnToString(&object->values[i+j], str), 0), sizeof(sprite_t), j); //TODO Fix
					}
					EditEntity(retVal, (EntityNumbers)i, checkSprite);
				} else if(CountMem(&object->values, sizeof(jsmntok_t)) == 2)
				{
					EditEntity(retVal, (EntityNumbers)i, ParseToVec2(checkObj, str));
				} else
				{
					printf("Error with : %s in JSON file", object->name);
				}
			} else if(checkTok->type == JSMN_STRING)
			{
				EditEntity(retVal, (EntityNumbers)i, JsmnToString(object->values, str));
			} else if(checkTok->type == JSMN_PRIMITIVE)
			{
				EditEntity(retVal, (EntityNumbers)i, (void*)StringToInt(JsmnToString(checkTok, str)));
			}else
			{
				printf("Error Unkown var %s", JsmnToString(checkTok, str));
			}
		}
	}
	return retVal;
}

vec2_t* ParseToVec2(object_t* object, char* str)
{
	vec2_t *retVal;
	retVal = (vec2_t*) malloc(sizeof(vec2_t));
	retVal->x = StringToInt(JsmnToString(&object->values[0], str));
	retVal->y = StringToInt(JsmnToString(&object->values[1], str));
	return retVal;
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
