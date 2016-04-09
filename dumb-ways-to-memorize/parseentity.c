#include "parseentity.h"
#include "parseobject.h"
#include "mystrings.h"
#include "audio.h"
#include <jsmn.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Globals
char *ComplexVariableNames[] = {"hazard(s)", "sound(s)", "collisionType", "entityState", 0};
char *Vector2VariableNames[] = {"accel", "velocity", "position", 0};
char *SimpleVariableNames[] = {"sprite(s)", "health", "damage", "height", "width", "frames", 0 };

void AddComplexMemToEnt(entity_t  *ent, entity_members_complex_t member, void *value)
{
	if(!value || !ent)
	{
		return;
	}

	switch(member)
	{
	case ENTITY_MEMBER_HAZARDS: ent->mHazards = (int) value; break;
	case ENTITY_MEMBER_COLLISION_TYPE: ent->mCollisionType = (collision_type_t) (int) value; break;
	case ENTITY_MEMBER_ENTITY_STATE: ent->mEntityState = (entity_state_t) (int) value; break;
	default:
		break;
	}
}

void AddVector2Entity(entity_t *ent, entity_members_vector2_t member, vec2_t *value)
{
	if(!value || !ent)
	{
		return;
	}

	switch(member)
	{
	case ENTITY_MEMBER_ACCEL: ent->mAccel = *value; break;
	case ENTITY_MEMBER_POSITION: ent->mPosition = *value; break;
	case ENTITY_MEMBER_VELOCITY: ent->mVelocity = *value; break;
	default:
		break;
	}
}

void AddSoundsToEnt(entity_t *ent, char **files, int group)
{
	sound_t *sound;
	if(!files || !ent)
	{
		return;
	}
	sound = LoadSound(files, (sound_mixer_group) group);
	ent->mSounds = sound;
}

void AddSpritesToEnt(entity_t *ent, char **files, int size)
{
	int i;
	sprite_t **checkSprite;
	if(!files || !ent)
	{
		return;
	}
	checkSprite = (sprite_t**) malloc(sizeof(sprite_t*)*(size+1));
	if(!checkSprite)
	{
		return;
	}
	for(i = 0; i < size; i++)
	{
		checkSprite[i] = LoadSprite(files[i], 0);
	}
	checkSprite[size] = NULL;
	ent->mSprites = checkSprite;
}

entity_t* ParseToEntity(object_t* object, char* str)
{
	int i, j, position, size, heights[MAX_ANIMATIONS] = {0}, widths[MAX_ANIMATIONS] = {0}, frames[MAX_ANIMATIONS] = {0};
	char **spriteFiles, *temp_str;
	entity_t *retVal;
	Frame *checkFrame = NULL;
	jsmntok_t *checkTok = NULL;
	object_t *checkObj = NULL;
	if(!object || !str)
		return NULL;
	retVal = (entity_t*) malloc(sizeof(entity_t));
	spriteFiles = (char**) malloc(sizeof(char*)*(MAX_ANIMATIONS+1));
	memset(spriteFiles, 0, sizeof(char*)*(MAX_ANIMATIONS+1));
	memset(retVal, 0, sizeof(entity_t));
	retVal->mName = object->name;
	for(i = 0; ComplexVariableNames[i]; i++)
	{
		if( (checkTok = FindKey(object->keys, ComplexVariableNames[i], str)) != NULL)
		{
			position = checkTok - object->keys;
			ParseComplexMember(retVal, &object->values[position], str, (entity_members_complex_t)i, 1);

			checkTok = NULL;
		} 
		else if( (checkObj = FindObject(object, ComplexVariableNames[i])) != NULL)
		{
			size = CountMem(checkObj->values, sizeof(jsmntok_t));
			ParseComplexMember(retVal, checkObj->values, str, (entity_members_complex_t)i, size);

			checkObj = NULL;
		}
	}

	for(i = 0; Vector2VariableNames[i]; i++)
	{
		if( (checkObj = FindObject(object, Vector2VariableNames[i])) != NULL)
		{
			AddVector2Entity(retVal, (entity_members_vector2_t) i, ParseToVec2(checkObj, str));
		}
	}

	for(i = 0; SimpleVariableNames[i]; i++)
	{
		if( (checkObj = FindObject(object, SimpleVariableNames[i])) != NULL)
		{
			for(j = 0; checkObj->values[j].type; j++)
			{
				switch(i)
				{
				case ENTITY_MEMBER_SPRITE: spriteFiles[j] = JsmnToString(&checkObj->values[j], str); break;
				case ENTITY_MEMBER_HEIGHT: JsmnToInt(&checkObj->values[j], str, &heights[j]); break;
				case ENTITY_MEMBER_WIDTH: JsmnToInt(&checkObj->values[j], str, &widths[j]); break;
				case ENTITY_MEMBER_FRAMES: JsmnToInt(&checkObj->values[j], str, &frames[j]); break;
				default:
					break;
				}
			}
		} else if ( (temp_str = FindValue(object, SimpleVariableNames[i], str)) != NULL)
		{
			switch(i)
				{
				case ENTITY_MEMBER_SPRITE: spriteFiles[0] = temp_str; break;
				case ENTITY_MEMBER_HEALTH: retVal->mHealth = StrToInt(temp_str); break;
				case ENTITY_MEMBER_DAMAGE: retVal->mDamage = StrToInt(temp_str); break;
				case ENTITY_MEMBER_HEIGHT: heights[0] = StrToInt(temp_str); break;
				case ENTITY_MEMBER_WIDTH: widths[0] = StrToInt(temp_str); break;
				case ENTITY_MEMBER_FRAMES: frames[0] = StrToInt(temp_str); break;
				default:
					break;
				}
		}
	}
	
	AddSpritesToEnt(retVal, spriteFiles, CountMem(spriteFiles, sizeof(char*)));

	if(retVal->mSprites)
	{
		for(i = 0; retVal->mSprites[i]; i++)
		{
			if(!retVal->mSprites[i])
				break;
			checkFrame = LoadAnimation(widths[i], heights[i], retVal->mSprites[i]->mSize.x, retVal->mSprites[i]->mSize.y);
			retVal->mSprites[i]->mSize.x = widths[i];
			retVal->mSprites[i]->mSize.y = heights[i];
			retVal->mSprites[i]->mFrames = frames[i] ? frames[i] : 1;
			memcpy(&retVal->mSprites[i]->mAnimations, checkFrame, sizeof(Frame)*retVal->mSprites[i]->mFrames );
		}
	}
	retVal->mNextThink = 1;
	retVal->mCurrentFrame = 0;
	retVal->mWeight = 1;
	retVal->Think = ThinkGeneric;
	retVal->Draw = DrawGeneric;
	retVal->Touch = TouchGeneric;
	PrintEntity(retVal);
	return retVal;
}

void ParseComplexMember(entity_t *ent, jsmntok_t* token, char *g_str, entity_members_complex_t member, int size)
{
	int i;
	char *temp, **sound_files;
	int checkInt = 0;
	if(!ent || !token || !g_str)
		return;
	if(member== ENTITY_MEMBER_HAZARDS)
	{
		for(i= 0; i < size; i++)
		{
			temp = JsmnToString(&token[i], g_str);
			checkInt += StrToHazard(temp);
			if(temp) free(temp);
		}
		AddComplexMemToEnt(ent, member, (void*)checkInt);
	} else if(member == ENTITY_MEMBER_SOUND)
	{
		//First string in list reserved to sound type
		temp = JsmnToString(token, g_str);
		checkInt = StrToSoundType(temp);
		if(temp) free(temp);
		if(checkInt != SOUND_GROUP_MAX)
		{
			sound_files = (char**) malloc(sizeof(char*)*size);
			for(i = 1; i < size-1; i++)
			{
				temp = JsmnToString(&token[i], g_str);
				if(!temp)
				{
					sound_files[i] = NULL;
					continue;
				}
				sound_files[i] = temp;
			}
			sound_files[size-1] = 0;
			AddSoundsToEnt(ent, sound_files, checkInt);
		}
	} else if(member == ENTITY_MEMBER_COLLISION_TYPE)
	{
		temp = JsmnToString(token, g_str);
		AddComplexMemToEnt(ent, member, (void*)StrToCollisionType(temp));
		if(temp) free(temp);
	} else if (member == ENTITY_MEMBER_ENTITY_STATE)
	{
		temp = JsmnToString(token, g_str);
		AddComplexMemToEnt(ent, member, (void*)StrToEntityState(temp));
		if(temp) free(temp);
	}
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
