#include "parsevariable.h"
#include "parseobject.h"
#include "mystrings.h"
#include <jsmn.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Globals
char *AssignableVariableNames[] = {"hazard(s)" , "health", "collisionType", "entityState", "sprite(s)", "accel", "velocity", "position", 0};
char *OtherVariableNames[] = { "height", "width", "frames", 0 };

//BIG ASS SWITCH CASE
void EditEntity(entity_t  *ent, EntityMembers member, void *value)
{
	if(value == NULL)
	{
		return;
	}
	switch(member)
	{
	case ENTITY_MEMBER_HAZARDS: ent->mHazards = (int) value; break;
	case ENTITY_MEMBER_HEALTH: ent->mHealth = (int) value; break;
	case ENTITY_MEMBER_COLLISION_TYPE: ent->mCollisionType = (collision_type_t) (int) value; break;
	case ENTITY_MEMBER_ENTITY_STATE: ent->mEntityState = (entity_state_t) (int) value; break;
	case ENTITY_MEMBER_SPRITES: ent->mSprites = (sprite_t**) value; break;
	case ENTITY_MEMBER_ACCEL: ent->mAccel = *((vec2_t*) value); break;
	case ENTITY_MEMBER_VELOCITY : ent->mVelocity = *((vec2_t*) value); break;
	case ENTITY_MEMBER_POSITION : ent->mPosition = *((vec2_t*) value); break;
	default:
		break;
	}
}

/**
 * Parse to entity.
 * Tries to Assign everything , but the entity function pointers.
 * Current Frame = 0, Weight = 1
 *
 * @param [in,out]	object	If non-null, the object.
 * @param [in,out]	str   	If non-null, the string.
 *
 * @return	null if it fails, else a pointer to an entity_t.
 *
 * @author	Anthony Rios
 * @date	3/21/2016
 */

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
	memset(retVal, 0, sizeof(entity_t));
	retVal->mName = object->name;
	for(i = 0; AssignableVariableNames[i]; i++)
	{
		if( (checkTok = FindKey(object->keys, AssignableVariableNames[i], str)) != NULL)
		{
			if(checkTok->type == JSMN_STRING)
			{
				if(i == ENTITY_MEMBER_HAZARDS || i == ENTITY_MEMBER_SPRITES)
				{
					checkInt = abs((int) object->keys - (int) checkTok)/sizeof(jsmntok_t);
					MiniParseFunc(retVal, &object->values[checkInt], str, (EntityMembers)i, 1);
					checkTok = NULL;
				} else
				{
					checkInt = abs((int) object->keys - (int) checkTok)/sizeof(jsmntok_t);
					MiniParseFunc(retVal, &object->values[checkInt], str, (EntityMembers)i, 1);
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
			if(i == ENTITY_MEMBER_HAZARDS || i == ENTITY_MEMBER_SPRITES)
			{
				MiniParseFunc(retVal, checkObj->values, str, (EntityMembers)i, CountMem(checkObj->values, sizeof(jsmntok_t)));
			} else if(CountMem(&checkObj->values, sizeof(jsmntok_t)) == 2)
			{
				EditEntity(retVal, (EntityMembers)i, ParseToVec2(checkObj, str));
			} else
			{
				printf("Error with : %s in JSON file \n", object->name);
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
		} else if ( (checkTok = FindKey(object->keys, OtherVariableNames[i], str)) != NULL)
		{
			switch(i)
				{
				case HEIGHT: JsmnToInt(checkTok, str, &heights[0]); break;
				case WIDTH: JsmnToInt(checkTok, str, &widths[0]); break;
				case FRAMES: JsmnToInt(checkTok, str, &frames[0]); break;
				default:
					break;
				}
		}
	}
	//retVal->mSprites = (sprite_t**) malloc(sizeof(sprite_t*)*(j+1));
	//retVal->mSprites[j] = NULL;
	if(retVal->mSprites)
	{
		for(i = 0; retVal->mSprites[i]; i++)
		{
			if(!retVal->mSprites[i])
				break;
			checkFrame = LoadAnimation(widths[i], heights[i], retVal->mSprites[i]->mSize.x, retVal->mSprites[i]->mSize.y);
			retVal->mSprites[i]->mFrames = frames[i] ? frames[i] : 1;
			memcpy(&retVal->mSprites[i]->mAnimations, checkFrame, sizeof(Frame)*retVal->mSprites[i]->mFrames );
		}
	}
	retVal->mNextThink = 1;
	retVal->mCurrentFrame = 0;
	retVal->Think = ThinkGeneric;
	retVal->Draw = DrawGeneric;
	retVal->Touch = TouchGeneric;
	PrintEntity(retVal);
	return retVal;
}

void MiniParseFunc(entity_t *ent, jsmntok_t* token, char *str, EntityMembers member, int size)
{
	int i;
	char *temp = NULL;
	int checkInt = 0;
	sprite_t **checkSprite = NULL;
	if(!ent || !token || !str)
		return;
	if(member== ENTITY_MEMBER_HAZARDS)
	{
		for(i= 0; i < size; i++)
		{
			temp = JsmnToString(&token[i], str);
			checkInt += StrToHazard(temp);
			if(temp) free(temp);
		}
		EditEntity(ent, member, (void*)checkInt);
	} else if(member== ENTITY_MEMBER_SPRITES)
	{
		checkSprite = (sprite_t**) malloc(sizeof(sprite_t*)*(size+1));
		if(!checkSprite)
		{
			return;
		}
		for(i = 0; i < size; i++)
		{
			temp = JsmnToString(&token[i], str);
			checkSprite[i] = LoadSprite(temp, 0);
			if(temp) free(temp);
		}
		memset(&checkSprite[size], 0, sizeof(sprite_t*));
		EditEntity(ent, member, checkSprite);
	} else if(member == ENTITY_MEMBER_COLLISION_TYPE)
	{
		temp = JsmnToString(token, str);
		EditEntity(ent, member, (void*)StrToCollisionType(temp));
		if(temp) free(temp);
	} else if (member == ENTITY_MEMBER_ENTITY_STATE)
	{
		temp = JsmnToString(token, str);
		EditEntity(ent, member, (void*)StrToEntityState(temp));
		if(temp) free(temp);
	}else
	{
		temp = JsmnToString(token, str);
		EditEntity(ent, member, (void*)StrToInt(temp));
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
	retVal->x = StrToInt(temp1);
	retVal->y = StrToInt(temp2);
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

void PrintEntity(entity_t *ent)
{
	int i;
	if(!ent)
	{
		printf("Print Entity given NULL Entity \n");
		return;
	}
	if(!ent->mName)
	{
		printf("Print Entity given NULL Entity Name");
		return;
	}
	printf("< Entity \n");
	printf("Entity Name : %s \n", ent->mName);
	printf("Entity CollisionType : %d \n", ent->mCollisionType);
	printf("Enity State : %d \n", ent->mEntityState);
	if(!ent->mSprites)
	{
		printf("No Sprites loaded \n");
		return;
	}
	if(!ent->mSprites[0])
	{
		printf("No Sprites loaded \n");
		return;
	}
	printf("Entity Sprites : %s \n", ent->mSprites[0]->name);
	for(i = 1; ent->mSprites[i]; i++)
	{
		printf(" %s ", ent->mSprites[i]->name);
	}
	printf(" Entity > \n");

}
