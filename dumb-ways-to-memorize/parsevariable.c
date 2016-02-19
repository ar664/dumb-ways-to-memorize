#include "parsevariable.h"
#include "parseobject.h"
#include "mystrings.h"
#include <jsmn.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Globals
char *AssignableVariableNames[] = {"hazard(s)" , "collisionType", "entityState", "sprite(s)", "accel", "velocity", "position", 0};
char *OtherVariableNames[] = { "height", "width", "frames", 0 };

//BIG ASS SWITCH CASE
void EditEntity(entity_t  *ent, EntityNumbers member, void *value)
{
	switch(member)
	{
	case HAZARDS: ent->mHazards = (int) value; break;
	case COLLISION_TYPE: ent->mCollisionType = (collision_type_t) (int) value; break;
	case ENTITY_STATE: ent->mEntityState = (entity_state_t) (int) value; break;
	case SPRITES: ent->mSprites = (sprite_t**) value; break;
	case ACCEL: ent->mAccel = *((vec2_t*) value); break;
	case VELOCITY : ent->mVelocity = *((vec2_t*) value); break;
	case POSITION : ent->mPosition = *((vec2_t*) value); break;
	default:
		break;
	}
}

entity_t* ParseToEntity(object_t* object, char* str)
{
	int i, j, checkInt, heights[MAX_ANIMATIONS] = {0}, widths[MAX_ANIMATIONS] = {0}, frames[MAX_ANIMATIONS] = {0};
	char *temp;
	entity_t *retVal;
	Frame *checkFrame = NULL;
	jsmntok_t *checkTok = NULL;
	object_t *checkObj = NULL;
	if(!object || !str)
		return NULL;
	retVal = (entity_t*) malloc(sizeof(entity_t));
	for(i = 0; AssignableVariableNames[i]; i++)
	{
		if( (checkTok = FindKey(object->keys, AssignableVariableNames[i], str)) != NULL)
		{
			if(checkTok->type == JSMN_STRING)
			{
				if(i == HAZARDS || i == SPRITES)
				{
					checkInt = abs((int) object->keys - (int) checkTok)/sizeof(jsmntok_t);
					MiniParseFunc(retVal, &object->values[checkInt], str, (EntityNumbers)i, 1);
					checkTok = NULL;
				} else
				{
					checkInt = abs((int) object->keys - (int) checkTok)/sizeof(jsmntok_t);
					MiniParseFunc(retVal, &object->values[checkInt], str, (EntityNumbers)i, 1);
					checkTok = NULL;
				}
			}else
			{
				temp = JsmnToString(checkTok, str);
				printf("Error Unkown var %s", temp);
				free(temp);
			}
		} else if( (checkObj = FindObject(object, AssignableVariableNames[i])) != NULL)
		{
			if(i == HAZARDS || i == SPRITES)
			{
				MiniParseFunc(retVal, checkObj->values, str, (EntityNumbers)i, CountMem(checkObj->values, sizeof(jsmntok_t)));
			} else if(CountMem(&checkObj->values, sizeof(jsmntok_t)) == 2)
			{
				EditEntity(retVal, (EntityNumbers)i, ParseToVec2(checkObj, str));
			} else
			{
				printf("Error with : %s in JSON file", object->name);
			}
		}
	}
	for(i = 0; OtherVariableNames[i]; i++)
	{
		if( (checkObj = FindObject(object, OtherVariableNames[i])) != NULL)
		{
			for(j = 0; checkObj->values[j].type; j++)
			{
				switch(i)
				{
				case HEIGHT: JsmnToInt(&checkObj->values[j], str, &heights[j]); break;
				case WIDTH: JsmnToInt(&checkObj->values[j], str, &widths[j]); break;
				case FRAMES: JsmnToInt(&checkObj->values[j], str, &frames[j]); break;
				default:
					break;
				}
			}
		}
	}
	for(i = 0; heights[i]; i++)
	{
		checkFrame = LoadAnimation(widths[i], heights[i], retVal->mSprites[i]->mSize.x, retVal->mSprites[i]->mSize.y);
		if(!retVal->mSprites[i])
			break;
		memcpy(&retVal->mSprites[i]->mAnimations, checkFrame, sizeof(Frame)*CountMem(checkFrame, sizeof(Frame)) );
		free(checkFrame);
	}
	
	return retVal;
}

void MiniParseFunc(entity_t *ent, jsmntok_t* token, char *str, EntityNumbers member, int size)
{
	int i;
	char *temp = NULL;
	int checkInt = 0;
	sprite_t **checkSprite = NULL;
	if(!ent || !token || !str)
		return;
	if(member== HAZARDS)
	{
		for(i= 0; i < size; i++)
		{
			temp = JsmnToString(token, str);
			checkInt += StrToHazard(temp);
			if(temp) free(temp);
		}
		EditEntity(ent, member, (void*)checkInt);
	} else if(member== SPRITES)
	{
		for(i= 0; i < size; i++)
		{
			temp = JsmnToString(token, str);
			AllocateDynamic((void**)&checkSprite, LoadSprite(temp, 0), sizeof(sprite_t*), i);
			if(temp) free(temp);
		}
		EditEntity(ent, member, checkSprite);
	} else if(member == COLLISION_TYPE)
	{
		temp = JsmnToString(token, str);
		EditEntity(ent, member, (void*)StrToCollisionType(temp));
		if(temp) free(temp);
	} else if (member == ENTITY_STATE)
	{
		temp = JsmnToString(token, str);
		EditEntity(ent, member, (void*)StrToEntityState(temp));
		if(temp) free(temp);
	}else
	{
		temp = JsmnToString(token, str);
		EditEntity(ent, member, (void*)StringToInt(temp));
		if(temp) free(temp);
	}
}

vec2_t* ParseToVec2(object_t* object, char* str)
{
	vec2_t *retVal;
	char *temp1, *temp2;
	retVal = (vec2_t*) malloc(sizeof(vec2_t));
	temp1 = JsmnToString(&object->values[0], str);
	temp2 = JsmnToString(&object->values[1], str);
	retVal->x = StringToInt(temp1);
	retVal->y = StringToInt(temp2);
	free(temp1); free(temp2);
	return retVal;
}

char **ParseToStringArray(object_t* object, char* str)
{
	int i, size;
	char *temp;
	char **retVal;
	if(!object || !str)
		return NULL;
	size = CountMem(object->values, sizeof(jsmntok_t));
	retVal = (char**) malloc(sizeof(char*)*(size+1));
	for(i = 0; i < size; i++)
	{
		temp = JsmnToString(&object->values[i], str);
		retVal[i] = temp;
	}
	retVal[size+1] = 0;
	return retVal;
}
