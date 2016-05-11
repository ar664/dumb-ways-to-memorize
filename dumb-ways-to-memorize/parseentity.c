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
void *FileSetFunctions[] = { AddSoundsToEnt, AddSpritesToEnt, 0};
char *DirectVariableNames[] = { "soundType", "spriteType", "hazard(s)", "collisionType", "entityState", "health", "damage", 0};
void *DirectConFunctions[] = { StrToSoundType, StrToSpriteType ,StrToHazard, StrToCollisionType, StrToEntityState, StrToInt, StrToInt, 0};
char *PhysicsVariableNames[] = { "weight", "accel", "velocity", "position", 0};
int PhysicsVariableSizes[] = { sizeof(int), sizeof(vec2_t), sizeof(vec2_t), sizeof(vec2_t), 0};
void *PhysicsConFunctions[] = { StrToInt, StrToInt, StrToInt, StrToInt, 0};
void *PhysicsSetFunctions[] = { cpBodySetMass, cpBodySetVelocity, cpBodySetVelocity, cpBodySetPosition, 0};
char *SpriteVariableNames[] = { "fps", "height", "width", "frames", 0};
//StrToInt

void *EntityAddFunctions[] = {0};

void AddSpriteMem(sprite_t *sprite, entity_members_sprite_t member, int value)
{
	int *var_to_edit;
	if(!sprite)
	{
		return;
	}
	var_to_edit = (int*) ((char*)sprite + SpriteVariableOffsets[member] );
	*var_to_edit = value;

}

//BOOM Bitch
void AddDirectMemToEnt(entity_t *ent, entity_members_direct_t member, int value)
{
	int *var_to_edit;
	if(!ent)
	{
		return;
	}
	var_to_edit = (int*) ((char*)ent + DirectVariableOffsets[member] );
	*var_to_edit = value;

}

//Assuming pre-converted
void AddPhysicsMemToEnt(entity_t *ent, entity_members_physics_t member, void *value)
{
	if(!ent || !value)
	{
		return;
	}
	((void(*)(void*, void*))PhysicsSetFunctions[member])(ent->mPhysicsProperties->body, value);
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
	int i, j, array_count;
	int temp_int, *temp_loc;

	//Files
	char **temp_array, *temp_str, *temp_arg;
	
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

	//Do Direct first for Compatibility with different file loading
	for(i = 0; DirectVariableNames[i]; i++)
	{
		temp_str = FindValue(object, DirectVariableNames[i], str);

		//Cast To Int returning Function
		temp_int =  ( (int(*)(char *))DirectConFunctions[i] )(temp_str);

		AddDirectMemToEnt(retVal, (entity_members_direct_t) i, temp_int);
		if(temp_str) free(temp_str);
	}

	//Load Files to Ent
	for(i = 0; FileVariableNames[i]; i++)
	{
		temp_str = FindValue(object, FileVariableNames[i], str);
		if(temp_str)
		{
			temp_array = (char**) malloc(sizeof(char*)*2);
			memset(temp_array, 0, sizeof(char*)*2);
			temp_array[0] = temp_str;
		} else
		{
			checkObj = FindObject(object, FileVariableNames[i]);
			if(!checkObj)
			{
				continue;
			}
			array_count = CountMem(object->values, sizeof(jsmntok_t));
			temp_array = (char**) malloc(sizeof(char*)*(array_count+1));
			memset(temp_array, 0, sizeof(char*)*(array_count+1));
			for(j = 0; j < array_count; j++)
			{
				temp_array[i] = JsmnToString(&object->values[j], str);
			}
		}
		if(temp_array)
		{
			//Get the flags we need from the entity
			temp_int = *(int*)( (char*) retVal  + DirectVariableOffsets[i]);

			//Load the files in with casted function
			( (void(*)(entity_t*, char**, int))FileSetFunctions[i] )(retVal, temp_array, temp_int);

			free(temp_array);
		}
	}

	// TODO: Process frame data after the loading of extra data
	if(!retVal->mSprites || !retVal->mSprites[0])
	{
		printf("Could not load sprites for entity: %s \n", retVal->mName);
		return NULL;
	}
	
	//Iteratre Through Sprites
	for(i = 0; retVal->mSprites[i]; i++)
	{
			
		//Iterate through members
		for(j = 0; SpriteVariableNames[j]; j++)
		{
			temp_str = FindValue(object, SpriteVariableNames[j], str);
			if ( temp_str )
			{
				temp_int = StrToInt(temp_str);
				AddSpriteMem(retVal->mSprites[i], (entity_members_sprite_t) j, temp_int);
			}
			else
			{
				checkObj = FindObject(object, SpriteVariableNames[j]);
				if(checkObj)
				{
					//Check if member value is set for current sprite
					if(i < CountMem(checkObj->values, sizeof(jsmntok_t)))
					{
						temp_arg = JsmnToString(&checkObj->values[i], str);
						temp_int = StrToInt(temp_arg);
						AddSpriteMem(retVal->mSprites[i], (entity_members_sprite_t) j, temp_int);
						if(temp_arg) free(temp_arg);
					}
				}
			
			}

			if(temp_str) free(temp_str);
		}
	}

	if(!AddPhyicsToEntity(retVal))
	{
		printf("Could not add entity %s to physics \n", retVal->mName);
		return NULL;
	}

	for(i = 0; PhysicsVariableNames[i]; i++)
	{
		temp_str = FindValue(object, PhysicsVariableNames[i], str);
		if(temp_str)
		{
			//Stuff
		} else
		{
			//Other Stuff
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
	case MEMBER_FILE: member->types.file = (entity_members_file_t)type; break;
	case MEMBER_PHYSICS: member->types.physics = (entity_members_physics_t)type; break;
	case MEMBER_SPRITE: member->types.sprite = (entity_members_sprite_t)type; break;
	case MEMBER_DIRECT: member->types.direct = (entity_members_direct_t)type; break;
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
	default:
		break;
	}

	return NULL;
}

entity_member_t* GetEntityMembers(object_t* object, char* str)
{
	int i, j, k, count, hit, hit_count;
	char *temp_str, **hit_values, **current_array;
	void *variable_arrays[] = { FileVariableNames, PhysicsVariableNames, SimpleVariableNames, 0};
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