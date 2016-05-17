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
void *PhysicsSetFunctions[] = { cpBodySetMass, cpBodySetVel, cpBodySetVel, cpBodySetPos, 0};
char *SpriteVariableNames[] = { "mpf", "height", "width", "frames", 0};
//StrToInt

void *EntityAddFunctions[] = {AddDirectMemToEnt, AddPhysicsMemToEnt, 0};

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
	cpFloat *temp_val;
	if(!ent || !value)
	{
		return;
	}
	temp_val = (cpFloat*)value;

	switch(member)
	{
	case(ENTITY_MEMBER_WEIGHT):
		{
			if(*temp_val == 0)
			{
				break;
			}
			((void(*)(cpBody*, cpFloat))PhysicsSetFunctions[member])(ent->mPhysicsProperties->body, *temp_val);
			break;
		}
	default:
		{
			((void(*)(cpBody*, cpVect))PhysicsSetFunctions[member])(ent->mPhysicsProperties->body, *(cpVect*)temp_val);
			break;
		}
		
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
	int i, j, array_count;
	int temp_int, *temp_loc;
	cpFloat temp_float[3] = {0};

	//Files
	char **temp_array, *temp_str, *temp_arg;
	
	//Used Vars
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
		temp_int =  ( (StrConFunc)DirectConFunctions[i] )(temp_str);

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
			if(!temp_array)
			{
				continue;
			}
			memset(temp_array, 0, sizeof(char*)*2);
			temp_array[0] = temp_str;
			array_count = 1;
		} else
		{
			checkObj = FindObject(object, FileVariableNames[i]);
			if(!checkObj)
			{
				continue;
			}
			array_count = CountMem(checkObj->values, sizeof(jsmntok_t));
			temp_array = (char**) malloc(sizeof(char*)*(array_count+1));
			if(!temp_array)
			{
				continue;
			}
			memset(temp_array, 0, sizeof(char*)*(array_count+1));
			for(j = 0; j < array_count; j++)
			{
				temp_array[j] = JsmnToString(&checkObj->values[j], str);
			}
		}
		if(temp_array)
		{
			//Get the flags we need from the entity
			temp_int = *(int*) ( (char*) retVal  + DirectVariableOffsets[i]);

			//Load the files in with casted function
			( ( FileSetFunc ) FileSetFunctions[i] )(retVal, temp_array, temp_int);

			for(j = 0; j < array_count; j++)
			{
				if(temp_array[j]) free(temp_array[j]);
				temp_array[j] = NULL;
			}
			free(temp_array);
			temp_array = NULL;
		}
	}

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
				checkObj = FindObject(object->children, SpriteVariableNames[j]);
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
		UpdateSprite(retVal->mSprites[i]);
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
			temp_float[0] = (cpFloat) StrToInt(temp_str);
			AddPhysicsMemToEnt(retVal, (entity_members_physics_t) i, temp_float);

		} else
		{
			checkObj = FindObject(object, PhysicsVariableNames[i]);
			if(checkObj)
			{
				temp_int = CountMem(checkObj->values, sizeof(jsmntok_t));
				for(j = 0; j < temp_int; j++)
				{
					temp_str = JsmnToString(&checkObj->values[i], str);
					temp_float[j] = (cpFloat)StrToInt(temp_str);
					if(temp_str) free(temp_str);
					temp_str = NULL;
				}
				AddPhysicsMemToEnt(retVal, (entity_members_physics_t)i, temp_float);
			}
		}
		//Reset float values
		memset(temp_float, 0, sizeof(cpFloat)*3);
		if(temp_str) free(temp_str);
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
	printf("Entity Sprites : \n %s \n", ent->mSprites[0]->name);
	for(i = 1; ent->mSprites[i]; i++)
	{
		printf(" %s \n", ent->mSprites[i]->name);
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
	int *retVal;
	if(!var)
	{
		return NULL;
	}
	switch(var_type)
	{
	case MEMBER_FILE:
		{
			return var;
		}
	case MEMBER_PHYSICS:
		{
			retVal = (int*) malloc(sizeof(int));
			if(!retVal)
			{
				return NULL;
			}
			*retVal = (cpFloat)StrToInt(var);
			return retVal;
		}
	case MEMBER_SPRITE:
		{
			retVal = (int*) malloc(sizeof(int));
			if(!retVal)
			{
				return NULL;
			}
			*retVal = StrToInt(var);
			return retVal;
		}
	case MEMBER_DIRECT:
		{
			retVal = (int*) malloc(sizeof(int));
			if(!retVal)
			{
				return NULL;
			}
			*retVal = ( ( int(*)(char*) ) DirectConFunctions[num_type]) (var);
			return retVal;
		}
	default:
		break;
	}

	return NULL;
}

entity_member_t* FindEntityMembers(object_t* object, char* str)
{
	int i, j, k, x, count, hit, hit_count, array_count;
	char *temp_str, *value_string, **current_array, **temp_array;
	void *variable_arrays[] = { FileVariableNames, PhysicsVariableNames, DirectVariableNames, 0};
	entity_member_t *members;
	if(!object || !str)
	{
		return NULL;
	}
	if(!object->keys)
	{
		return NULL;
	}

	//Count KV Pairs and Prepare to Collect Hits
	count = CountMem(object->keys, sizeof(jsmntok_t));
	members = (entity_member_t*) malloc(sizeof(entity_member_t)*(count+1));
	if(!members)
	{
		return NULL;
	}

	memset(members, 0, sizeof(entity_member_t)*(count+1));
	
	hit = 0; hit_count = 0;
	value_string = NULL;

	//Iterate through single value keys
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
			current_array = (char**) variable_arrays[j];
			for(k = 0; current_array[k]; k++)
			{
				if(!strcmp(temp_str, current_array[k]))
				{
					hit = 1;
					value_string = JsmnToString(&object->values[i], str);
					members[hit_count].data = ConvertType( j, k,  value_string );
					members[hit_count].member_type = j;
					EntityMemberSetType(&members[hit_count], k);
					break;
				}
			}

			if(hit)
			{
				hit = 0;
				hit_count++;
				if(value_string) free(value_string);
				if(temp_str) free(temp_str);
				break;
			}
		}

	}

	count = CountMem(object->children, sizeof(object_t));

	//Check if no valueable targets
	if( (hit_count + count) == 0)
	{
		if(members) free(members);
		return NULL;
	}

	members = (entity_member_t*) realloc(members, sizeof(entity_member_t)*(hit_count+count+1));
	if(!members)
	{
		return NULL;
	}

	for(i = 0; i < count; i++)
	{
		temp_str = object->children[i].name;
		if(!temp_str)
		{
			continue;
		}

		for(j = 0; variable_arrays[j]; j++)
		{
			current_array = (char**) variable_arrays[j];
			for(k = 0; current_array[k]; k++)
			{
				if(!strcmp(temp_str, current_array[k]))
				{
					hit = 1;
					array_count = CountMem(object->children[i].values, sizeof(jsmntok_t));
					temp_array = (char**) malloc(sizeof(char*)*(array_count+1));
					if(!temp_array)
					{
						hit = 0;
						break;
					}
					for(x = 0; x < array_count; x++)
					{
						value_string = JsmnToString(&object->children[i].values[x], str);
						temp_array[x] = (char*) ConvertType(j, k, value_string );
						if(value_string) free(value_string);
					}
					temp_array[array_count] = NULL;
					members[hit_count].data = temp_array;
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

	//Check if we never hit a value
	if(hit_count == 0)
	{
		if(members) free(members);
		return NULL;
	}

	memset(&members[hit_count], 0, sizeof(entity_member_t));
	return members;
}

void ApplyEntityMember(entity_t *ent, entity_member_t *member)
{
	
	if(!member || !ent)
	{
		return;
	}
	if(!member->data)
	{
		return;
	}

	if(member->member_type == MEMBER_PHYSICS)
	{
		AddPhysicsMemToEnt(ent, member->types.physics, member->data);
	} else if (member->member_type == MEMBER_DIRECT)
	{
		AddDirectMemToEnt(ent, member->types.direct, *(int*)member->data);
	}
}

void ApplyEntityMembers(entity_t *ent, entity_member_t* member)
{
	int i, count;
	if(!ent || !member)
	{
		return;
	}
	count = CountMem(member, sizeof(entity_member_t));
	for(i = 0; i < count; i++)
	{
		ApplyEntityMember(ent, &member[i]);
	}
}

void FreeEntityMembers(entity_member_t *member)
{
	int i, count;
	if(!member)
	{
		return;
	}
	count = CountMem(member, sizeof(entity_member_t));
	for(i = 0; i < count; i++)
	{
		if(member[i].data)
		{
			free(member[i].data);
			member[i].data = NULL;
		}
	}

	free(member);
}