#include "parseentity.h"
#include "parseobject.h"
#include "mystrings.h"
#include "audio.h"
#include <jsmn.h>
#include "dumb_physics.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Globals
char *FileVariableNames[] = {  "sound(s)", "sprite(s)",  0};

char *DynamicVariableNames[] = { "soundType", "spriteType", "hazard(s)", "collisionType", "entityState", 0};
void *DynamicFunctions[] = { StrToSoundType, StrToSpriteType ,StrToHazard, StrToCollisionType, StrToEntityState, 0};
char *Vector2VariableNames[] = {"accel", "velocity", "position", 0};
//ParseToVec
char *SimpleVariableNames[] = {"health", "weight", "fps", "damage", "height", "width", "frames",  0 };
//StrToInt

void *EntityAddFunctions[] = {0};

void AddDynamicMemToEnt(entity_t  *ent, entity_members_dynamic_t member, int value)
{
	if(!value || !ent)
	{
		return;
	}

	switch(member)
	{
	case ENTITY_MEMBER_HAZARDS: ent->mHazards = value; break;
	case ENTITY_MEMBER_COLLISION_TYPE: ent->mCollisionType = (collision_type_t) value; break;
	case ENTITY_MEMBER_ENTITY_STATE: ent->mEntityState = (entity_state_t) value; break;
	default:
		break;
	}
}

void AddVector2Entity(entity_t *ent, entity_members_vector2_t member, vec2_t *value)
{
	cpVect *temp_val;
	if(!value || !ent)
	{
		return;
	}

	temp_val = (cpVect*)Vec2Cp(value);
	if(!ent->mPhysicsProperties->body || !temp_val)
	{
		return;
	}

	switch(member)
	{
	case ENTITY_MEMBER_ACCEL: cpBodyApplyImpulse(ent->mPhysicsProperties->body, *temp_val, cpvzero); break;
	case ENTITY_MEMBER_POSITION: cpBodySetPos(ent->mPhysicsProperties->body, *temp_val ); break;
	case ENTITY_MEMBER_VELOCITY: cpBodySetVel(ent->mPhysicsProperties->body, *temp_val ); break;
	default:
		break;
	}

	if(temp_val) free(temp_val);
}

void AddSimpleMemToEnt(entity_t *ent, entity_members_simple_t member, int value)
{
	if(!ent)
	{
		return;
	}

	switch(member)
	{
	case ENTITY_MEMBER_HEALTH: ent->mHealth = value; break;
	case ENTITY_MEMBER_WEIGHT: ent->mPhysicsProperties ? cpBodySetMass(ent->mPhysicsProperties->body, value) : NULL; break;
	case ENTITY_MEMBER_FPS: ent->mSprites ? ent->mSprites[0]->mMillisecondsPerFrame = 1000/(value+1) : NULL; break;
	case ENTITY_MEMBER_DAMAGE: ent->mDamage = value;

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

void AddSpritesToEnt(entity_t *ent, char **files, int flags)
{
	int i, count;
	sprite_t **checkSprite;
	if(!files || !ent)
	{
		return;
	}
	count = CountMem(files, sizeof(char**));
	checkSprite = (sprite_t**) malloc(sizeof(sprite_t*)*(count+1));
	if(!checkSprite)
	{
		return;
	}
	for(i = 0; i < count; i++)
	{
		checkSprite[i] = LoadSprite(files[i], 0);
	}
	checkSprite[count] = NULL;
	ent->mSprites = checkSprite;
}

entity_t* ParseToEntity(object_t* object, char* str)
{
	int i, j, array_count, heights[MAX_ANIMATIONS] = {0}, widths[MAX_ANIMATIONS] = {0}, frames[MAX_ANIMATIONS] = {0};

	//Members
	int dynamic_members[ENTITY_MEMBER_DYNAMIC_MAX] = {0}, simple_members[ENTITY_MEMBER_SIMPLE_MAX] = {0};
	vec2_t vector_members[ENTITY_MEMBER_VECTOR_MAX] = {0};

	//Files
	char **spriteFiles, **soundFiles, *temp_str;
	void *file_members[ENTITY_MEMBER_FILE_MAX] = { spriteFiles, soundFiles};
	
	//Used Vars
	vec2_t *temp_vec;
	Frame *checkFrame = NULL;
	object_t *checkObj = NULL;
	entity_t *retVal;

	if(!object || !str)
		return NULL;
	retVal = (entity_t*) malloc(sizeof(entity_t));
	memset(retVal, 0, sizeof(entity_t));
	retVal->mName = object->name;

	for(i = 0; SimpleVariableNames[i]; i++)
	{
		temp_str = FindValue(object, SimpleVariableNames[i], str);
		if ( temp_str )
		{
			simple_members[i] = StrToInt(temp_str);
		}
		else
		{
			checkObj = FindObject(object, SimpleVariableNames[i]);
			if(checkObj)
			{
				for(j = 0; checkObj->values[j].type; j++)
				{
					switch(i)
					{
					case ENTITY_MEMBER_HEIGHT: JsmnToInt(&checkObj->values[j], str, &heights[j]); break;
					case ENTITY_MEMBER_WIDTH: JsmnToInt(&checkObj->values[j], str, &widths[j]); break;
					case ENTITY_MEMBER_FRAMES: JsmnToInt(&checkObj->values[j], str, &frames[j]); break;
					default:
						break;
					}
				}
			}
			
		}

		if(temp_str) free(temp_str);
	}

	for(i = 0; DynamicVariableNames[i]; i++)
	{
		temp_str = FindValue(object, DynamicVariableNames[i], str);
		dynamic_members[i] =  ((int(*)(char *))DynamicFunctions[i])(temp_str);
		if(temp_str) free(temp_str);
	}

	for(i = 0; FileVariableNames[i]; i++)
	{
		temp_str = FindValue(object, FileVariableNames[i], str);
		if(temp_str)
		{
			file_members[i] = malloc(sizeof(char*)*2);
			memset(file_members[i], 0, sizeof(char*)*2);
			file_members[0] = temp_str;
		} else
		{
			checkObj = FindObject(object, FileVariableNames[i]);
			if(!checkObj)
			{
				continue;
			}
			array_count = CountMem(object->values, sizeof(jsmntok_t));
			file_members[i] = malloc(sizeof(char*)*(array_count+1));
			memset(file_members[i], 0, sizeof(char*)*(array_count+1));
			for(j = 0; j < array_count; j++)
			{
				file_members[i] = JsmnToString(&object->values[j], str);
			}
		}
	}


	AddSpritesToEnt(retVal, (char**) file_members[i], 0);
	AddSoundsToEnt(retVal, (char**) file_members[i], 0);

	if(!retVal->mSprites || !retVal->mSprites[0])
	{
		printf("Could not load sprites for entity: %s \n", retVal->mName);
		return NULL;
	}

	for(i = 0; retVal->mSprites[i]; i++)
	{
		if(!retVal->mSprites[i])
			break;
		checkFrame = LoadAnimation(widths[i], heights[i], retVal->mSprites[i]->mRawSize.x, retVal->mSprites[i]->mRawSize.y);
		retVal->mSprites[i]->mSize.x = widths[i] ? widths[i] : retVal->mSprites[i]->mRawSize.y;
		retVal->mSprites[i]->mSize.y = heights[i] ? heights[i] : retVal->mSprites[i]->mRawSize.y;
		retVal->mSprites[i]->mFrames = frames[i] ? frames[i] : 1;
		retVal->mSprites[i]->mMillisecondsPerFrame = simple_members[ENTITY_MEMBER_FPS] ? 1000/simple_members[ENTITY_MEMBER_FPS]  : 1000/DRAW_FRAME_DELAY;
		memset(&retVal->mSprites[i]->mAnimations[0], 0, sizeof(Frame)*MAX_ANIMATIONS);
		if(checkFrame)
		{
			memcpy(&retVal->mSprites[i]->mAnimations[0], checkFrame, sizeof(Frame)*(retVal->mSprites[i]->mFrames+1));
		}
		if(checkFrame) free(checkFrame);
	}

	if(!AddPhyicsToEntity(retVal))
	{
		printf("Could not add entity %s to physics \n", retVal->mName);
		return NULL;
	}
	cpBodySetMass(retVal->mPhysicsProperties->body, (float) (simple_members[ENTITY_MEMBER_WEIGHT]  ? simple_members[ENTITY_MEMBER_WEIGHT] : 1));

	for(i = 0; Vector2VariableNames[i]; i++)
	{
		if( (checkObj = FindObject(object, Vector2VariableNames[i])) != NULL)
		{
			temp_vec = ParseToVec2(checkObj, str);
			AddVector2Entity(retVal, (entity_members_vector2_t) i, temp_vec);
			if(temp_vec) free(temp_vec);
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

void EntityMemberSetType(entity_member_t *member, int type)
{
	switch(member->member_type)
	{
	case MEMBER_COMPLEX: member->types.complex = (entity_members_file_t)type; break;
	case MEMBER_VECTOR: member->types.vector = (entity_members_vector2_t)type; break;
	case MEMBER_SIMPLE: member->types.simple = (entity_members_simple_t)type; break;
	default:
		break;
	}
}

void *ConvertType(int var_type, int num_type, char *var)
{
	if(!var)
	{
		return NULL;
	}
	switch(var_type)
	{
	case MEMBER_COMPLEX:
		{
			break;
		}
	case MEMBER_VECTOR:
		{
			break;
		}
	case MEMBER_SIMPLE:
		{
			break;
		}
	}

	return NULL;
}

entity_member_t* GetEntityMembers(object_t* object, char* str)
{
	int i, j, k, count, hit, hit_count;
	char *temp_str, **hit_values, **current_array;
	void *variable_arrays[] = { FileVariableNames, Vector2VariableNames, SimpleVariableNames, 0};
	entity_member_t *members;
	if(!object || !str)
	{
		return NULL;
	}
	if(!object->keys)
	{
		return NULL;
	}

	//Count Objects and Prepare to Collect Hits
	count = CountMem(object->keys, sizeof(jsmntok_t));
	hit_values = (char**) malloc(sizeof(char*)*(count+1));
	memset(hit_values, 0, sizeof(char*)*(count+1));
	members = (entity_member_t*) malloc(sizeof(entity_member_t)*(count+1));
	memset(members, 0, sizeof(entity_member_t)*(count+1));
	
	hit = 0; hit_count = 0;

	for(i = 0; i < count; i++)
	{
		temp_str = JsmnToString(&object->keys[i], str);
		if(!temp_str)
		{
			continue;
		}

		//Iterate through variable names

		for(j = 0; variable_arrays[j]; j++)
		{
			current_array = (char**) variable_arrays;
			for(k = 0; current_array[k]; k++)
			{
				if(!strcmp(temp_str, current_array[k]))
				{
					hit = 1;
					hit_values[i] = JsmnToString(&object->keys[i], str);
					members[hit_count].member_type = j;
					EntityMemberSetType(&members[hit_count], k);
					break;
				}
			}

			if(hit)
			{
				hit = 0;
				hit_count++;
				if(temp_str) free(temp_str);
				break;
			}
		}

	}


	return NULL;
}