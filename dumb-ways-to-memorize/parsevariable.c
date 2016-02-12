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
	int i, j, checkInt = 0, heights[MAX_ANIMATIONS] = {0}, widths[MAX_ANIMATIONS] = {0}, frames[MAX_ANIMATIONS] = {0};
	char *temp;
	entity_t *retVal;
	sprite_t **checkSprite = NULL;
	Frame *checkFrame = NULL;
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
					{
						temp = JsmnToString(&checkObj->values[j], str);
						checkInt += StrToHazard(temp);
						free(temp);
					}
					EditEntity(retVal, (EntityNumbers)i, (void*)StrToHazard(Hazards_str[i]));
				} else if(i == SPRITES)
				{
					for(j = 0; j < checkTok->size; j++)
					{
						temp = JsmnToString(&object->values[j], str);
						AllocateDynamic((void**)&checkSprite, LoadSprite(temp, 0), sizeof(sprite_t*), j);
						free(temp);
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
				EditEntity(retVal, (EntityNumbers)i, JsmnToString(checkTok, str));
			} else if(checkTok->type == JSMN_PRIMITIVE)
			{
				temp = JsmnToString(checkTok, str);
				EditEntity(retVal, (EntityNumbers)i, (void*)StringToInt(temp));
				free(temp);
			}else
			{
				temp = JsmnToString(checkTok, str);
				printf("Error Unkown var %s", temp);
				free(temp);
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
				case 0: 
					temp = JsmnToString(&checkObj->values[j], str);
					heights[j] = StringToInt(temp);
					free(temp);
					//retVal->mSprites[j]->mAnimations[j] = LoadAnimation();
					break;
				case 1:
					temp = JsmnToString(&checkObj->values[j], str);
					widths[j] = StringToInt(temp);
					free(temp);
					break;
				case 2:
					temp = JsmnToString(&checkObj->values[j], str);
					frames[j] = StringToInt(temp);
					free(temp);
					break;
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
	size = CountMem(object->values, sizeof(jsmntok_t));
	retVal = (char**) malloc(sizeof(char*)*(size+1));
	for(i = 0; i < size; i++)
	{
		temp = JsmnToString(&object->values[i], str);
		retVal[i] = temp;
	}
	return retVal;
}
