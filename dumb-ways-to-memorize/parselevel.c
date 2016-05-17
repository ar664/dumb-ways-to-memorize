#include "entity.h"
#include "player.h"
#include "parselevel.h"
#include "parseobject.h"
#include "parseentity.h"
#include "ai_interpret.h"
#include "mystrings.h"
#include "dumb_physics.h"
#include <stdio.h>
#include <string.h>

level_t *gCurrentLevel = NULL;  
char *LevelGlobalObjectNames[] = {"Enemies", "Objects", "Spawn", "Playlist", 0};
char *LevelLocalObjectNames[] = {"enemy", "object", 0};
char *LevelGlobalOptionNames[] = {"Background", "Hint", 0};
char *LevelLocalOptionNames[] = {"position", "variables", "ai", "extra", "tile", 0};

int LoadLevel(object_t *level, char *g_str)
{
	jsmntok_t *temp_tok;
	object_t *temp_obj;
	int i;
	char *temp_str = NULL;
	if(!level || !level->keys)
	{
		printf("Could not load NULL lvl");
		return -1;
	}
	if(!gCurrentLevel)
	{
		gCurrentLevel = (level_t*) malloc(sizeof(level_t));
	} else {
		FreeNonPlayerEntities();
	}

	memset(gCurrentLevel, 0, sizeof(level_t));
	//Assign Name & quit if no name
	temp_str = FindValue(level, G_NAME_STR, g_str);
	if(!temp_str)
	{
		printf("Level %s has no Name \n", level->name);
		return -1;
	}
	gCurrentLevel->mName = temp_str;
	
	for(i = 0; i < LEVEL_G_OPTION_MAX; i++)
	{
		temp_tok = FindValueToken(level, LevelGlobalOptionNames[i], g_str);
		if(!temp_tok)
		{
			continue;
		}
		AddGlobalOption(gCurrentLevel, temp_tok, g_str, (level_global_option_t) i);
	}

	for(i = 0; i < LEVEL_G_OBJECT_MAX; i++)
	{
		temp_obj = FindObject(level, LevelGlobalObjectNames[i]);
		if(!temp_obj)
		{
			continue;
		}
		AddGlobalObject(gCurrentLevel, temp_obj, g_str, (level_global_object_t)i);
	}

	return 0;
}

int LoadGameState()
{
	FILE *file;
	vec2_t player_pos;
	char buffer[256], level_name[128], *temp_str;
	int i, j, levels, powers, used_powers;
	if(!gLevels | !gPowerUpsFile)
	{
		return -1;
	}
	//Open File
	file = fopen(SAVE_FILE, "rb");
	if(!file)
	{
		printf("Could not open save file. \n");
		return -1;
	}

	//Read Position
	fread(&player_pos.x, sizeof(int), 1, file);
	fread(&player_pos.y, sizeof(int), 1, file);
	if(fgetc(file) != '\n' )
	{
		return -1;
	}

	//Read Levels
	// Get Level Name
	i = 0;
	while( !feof(file) )
	{
		buffer[i] = fgetc(file);
		if(i > 127)
		{
			return -1;
		}
		if(buffer[i] == ' ')
		{
			break;
		}
		i++;
	}

	memcpy(level_name, buffer, sizeof(char)*i);
	level_name[i] = NULL;
	memset(buffer, 0, sizeof(char)*256);

	//Get Level Count
	levels = fgetc(file);
	if( fgetc(file) != '\n' )
	{
		return -1;
	}
	gSelectedLevels = (char**) malloc(sizeof(char*)*(levels+1));
	for(i = 0; i < levels; i++)
	{
		j = 0;
		//Get Level File Locations
		while( !feof(file) )
		{
			buffer[j] = fgetc(file);
			if(j > 255)
			{
				return -1;
			}
			if(buffer[j] == '\n')
			{
				break;
			}
			j++;
		}
		buffer[j] = NULL;
		temp_str = (char*) malloc(sizeof(char)*(j+1));
		memset(temp_str, 0, sizeof(char)*j);
		if(!temp_str)
		{
			return -1;
		}
		memcpy(temp_str, buffer, sizeof(char)*j);
		temp_str[j] = NULL;
		//Add to selected levels
		gSelectedLevels[i] = temp_str;
		memset(buffer, 0, sizeof(char)*256);
	}
	gSelectedLevels[levels] = NULL;

	powers = fgetc(file);
	if( fgetc(file) != '\n' )
	{
		return -1;
	}

	used_powers = 0;
	gSelectedPowerUps = (char**) malloc(sizeof(char*)*(powers+1));
	gUsedPowerUps = (char**) malloc(sizeof(char*)*(powers+1));
	memset(gSelectedPowerUps, 0, sizeof(char*)*(powers+1));
	memset(gUsedPowerUps, 0, sizeof(char*)*(powers+1));
	for(i = 0; i < powers; i++)
	{
		j = 0;
		while( !feof(file) )
		{
			buffer[j] = fgetc(file);
			if(j > 255)
			{
				return -1;
			}
			if(buffer[j] == ' ')
			{
				break;
			}
			j++;
		}
		buffer[j] = NULL;
		temp_str = (char*) malloc(sizeof(char)*j);
		memset(temp_str, 0, sizeof(char)*j);
		if(!temp_str)
		{
			return -1;
		}
		memcpy(temp_str, buffer, sizeof(char)*(j-1));
		gSelectedPowerUps[i] = temp_str;
		memset(buffer, 0, sizeof(char)*256);
		if(fgetc(file) == '1')
		{
			gUsedPowerUps[used_powers] = gSelectedPowerUps[i];
			used_powers++;
		}
		if(fgetc(file) != '\n')
		{
			break;
		}

	}
	gUsedPowerUps[powers] = NULL;

	for(i = 0; i < levels; i++)
	{
		ConvertFileToUseable(gSelectedLevels[i], NULL, &gLevelData, &gLevelTokens);
		if(!gLevelData || !gLevelTokens )
		{
			return -1;
		}
		gLevelObject = ParseToObject(gLevelTokens, gLevelData);
		if(!gLevelObject)
		{
			return -1;
		}
		LoadLevel(gLevelObject, gLevelData);
		if(!strcmp(gCurrentLevel->mName, level_name))
		{
			break;
		}
	}
	//Add in the player
	InitPlayer();
	InitCursor();
	if(!gPlayer || !gCursor)
	{
		return -1;
	}
	gPlayer->mPhysicsProperties->body->p.x = player_pos.x;
	gPlayer->mPhysicsProperties->body->p.y = player_pos.y;
	return 0;
}

void SpawnInLevel();


void TileLevelEntity(entity_t* ent, vec2_t* start_position, vec2_t* end_position)
{
	int x,y;
	vec2_t *count_pos, new_pos;
	cpVect cpPos;
	entity_t *temp_ent;
	if(!ent || !end_position)
	{
		return;
	}
	if(!start_position)
	{
		start_position = (vec2_t*) malloc(sizeof(vec2_t));
		memset(start_position,0, sizeof(vec2_t));
	}

	count_pos = (vec2_t*) malloc(sizeof(vec2_t));
	count_pos->x = (end_position->x - start_position->x)/ent->mSprites[0]->mSize.x;
	if(!count_pos->x)
	{
		count_pos->x = 1;
	}
	count_pos->y = (end_position->y - start_position->y)/ent->mSprites[0]->mSize.y;
	if(!count_pos->y)
	{
		count_pos->y = 1;
	}
	for(x = 0; x < count_pos->x; x++)
	{
		for(y = 0; y < count_pos->y; y++)
		{
			//Check to not respawn first entity
			if(x == 0 && y == 0)
			{
				continue;
			}
			temp_ent = InitNewEntity();
			if(!temp_ent)
			{
				printf("Max Entities reached in level \n");
				continue;
			}
			
			memcpy(temp_ent, ent, sizeof(entity_t));
			new_pos.x = start_position->x + x*ent->mSprites[0]->mSize.x;
			new_pos.y = start_position->y + y*ent->mSprites[0]->mSize.y;
			cpPos.x = (cpFloat) new_pos.x;
			cpPos.y = (cpFloat) new_pos.y;
			AddPhyicsToEntity(temp_ent);
			if(!temp_ent->mPhysicsProperties)
			{
				return;
			}
			cpBodySetPos(temp_ent->mPhysicsProperties->body, cpPos);
			new_pos.x = 0; new_pos.y = 0;
			AddEntityToPhysics(temp_ent);
			AddCollisionHandlerToEntity(ent);
		}
	}
}

void LevelMusicNext()
{
	int i, songs, currentSong = 0;
	level_t *level = gCurrentLevel;
	if(!level)
	{
		return;
	}
	if(!level->mMusic)
	{
		return;
	}
	songs = CountMem(level->mMusic, sizeof(sound_t*));
	if(!level->mCurrentSong)
	{
		level->mCurrentSong = level->mMusic[0];
	} else
	{
		for(i = 0; i < songs; i++)
		{
			if(level->mCurrentSong == level->mMusic[i])
			{
				currentSong = i;
				break;
			}
		}
		if(currentSong >= songs)
		{
			level->mCurrentSong = level->mMusic[0];
		} else
		{
			level->mCurrentSong = level->mMusic[currentSong];
		}
	}
	if(!level->mCurrentSong)
	{
		return;
	}
	Mix_PlayMusic(level->mCurrentSong->music, 0);
	Mix_HookMusicFinished(LevelMusicNext);
}

int SaveGameState()
{
	FILE *file;
	vec2_t temp_pos;
	int i, j, levels, power_ups, used_power_ups, used_bool;
	char buffer_int[32];
	if(!gCurrentLevel || !gSelectedLevels || !gUsedPowerUps || !gSelectedPowerUps || !gPlayer)
	{
		return -1;
	}
	printf("Saving Current Game State... \n");
	file = fopen(SAVE_FILE, "wb");
	if(!file)
	{
		printf("Unable to save to file. \n");
		return -1;
	}
	
	//Get Position
	temp_pos.x = (int) gPlayer->mPhysicsProperties->body->p.x;
	temp_pos.y = (int) gPlayer->mPhysicsProperties->body->p.y;

	//Write Position
	/* Could be useable code
	memset(buffer_int, 0, sizeof(char)*32);
	sprintf(buffer_int, "%d", temp_pos.x);
	*/
	fwrite(&temp_pos.x, sizeof(int), 1, file);
	fwrite(&temp_pos.y, sizeof(int), 1, file);
	fprintf(file , "\n");

	//Write Level Data
	levels = CountMem(gSelectedLevels, sizeof(char*));
	fprintf(file, gCurrentLevel->mName);
	fprintf(file, " %c\n", levels);
	for(i = 0; i < levels; i++)
	{
		fprintf(file, gSelectedLevels[i]);
		fprintf(file, "\n");
	}
	
	//Write PowerUp Data
	power_ups = CountMem(gSelectedPowerUps, sizeof(char*));
	used_power_ups = CountMem(gUsedPowerUps, sizeof(char*));
	fprintf(file, "%c\n", power_ups);
	for(i = 0; i < power_ups; i++)
	{
		fprintf(file, gSelectedPowerUps[i]);
		fprintf(file, " ");
		used_bool = 0;
		for(j = 0; j < used_power_ups; j++)
		{
			if(strcmp(gSelectedPowerUps[i], gUsedPowerUps[j]))
			{
				used_bool = 1;
				break;
			}
		}
		fprintf(file, "%d\n", used_bool);
	}
	fprintf(file, "%c", EOF);
	fclose(file);
	return 0;
}

void DrawLevel()
{
	if(!gCurrentLevel)
	{
		printf("Level not loaded, unable to print");
		return;
	}
	DrawSprite(gCurrentLevel->mBackground, NULL , NULL, gRenderer, 0);
}

void AddGlobalObject(level_t* level, object_t* obj, char* g_str, level_global_object_t type)
{
	char *temp_str;
	vec2_t *temp_vec;
	sound_t *temp_sound;
	int i, objects;
	switch(type)
	{
	case(LEVEL_G_OBJECT_ENEMIES):
		{
			objects = CountMem(obj->children, sizeof(object_t));
			for(i = 0; i < objects; i++)
			{
				AddLocalObject(level, &obj->children[i], g_str, LEVEL_L_OBJECT_ENEMY);
			}
			break;
		}
	case(LEVEL_G_OBJECT_OBJECTS):
		{
			objects = CountMem(obj->children, sizeof(object_t));
			for(i = 0; i < objects; i++)
			{
				AddLocalObject(level, &obj->children[i], g_str, LEVEL_L_OBJECT_OBJECT);
			}
			break;
		}
	case(LEVEL_G_OBJECT_PLAYLIST):
		{
			objects = CountMem(obj->values, sizeof(jsmntok_t));
			level->mMusic = (sound_t**) malloc(sizeof(sound_t*)*(objects+1));
			memset(level->mMusic, 0, sizeof(sound_t*)*(objects+1));
			for(i = 0; i < objects; i++)
			{
				temp_str = JsmnToString(&obj->values[i], g_str);
				if(!temp_str)
				{
					continue;
				}
				temp_sound = LoadSound(&temp_str, SOUND_GROUP_MUSIC);
				if(!temp_sound)
				{
					continue;
				}
				level->mMusic[i] = temp_sound;

			}
			level->mCurrentSong = level->mMusic[0];
			level->mMusic[objects] = NULL;
			LevelMusicNext();
			break;
		}
	case(LEVEL_G_OBJECT_SPAWN):
		{
			temp_vec = ParseToVec2(obj, g_str);
			if(!temp_vec)
			{
				break;
			}
			level->mSpawnPoint = *temp_vec;
			if(temp_vec) free(temp_vec);
			break;
		}
	default:
		break;
	}
		
}

void AddLocalObject(level_t* level, object_t* obj, char* g_str, level_local_object_t type)
{
	int i;
	char *obj_name;
	object_t *temp_obj;
	jsmntok_t *temp_tok;
	entity_t *temp_ent, *cached_ent;
	switch(type)
	{
	case(LEVEL_L_OBJECT_ENEMY):
		{
			obj_name = FindValue(obj, LevelLocalObjectNames[LEVEL_L_OBJECT_ENEMY], g_str);
			if(!obj_name)
			{
				break;
			}
			cached_ent = FindCachedEntity(obj_name);
			if(!cached_ent)
			{
				break;
			}
			temp_ent = InitNewEntity();
			if(!temp_ent)
			{
				break;
			}
			memcpy(temp_ent, cached_ent, sizeof(entity_t));

			//Set Think Functions
			temp_ent->Think = ThinkEnemy;
			temp_ent->Touch = TouchEnemy;
			temp_ent->Draw = DrawGeneric;

			//Set Extra Options
			AddPhyicsToEntity(temp_ent);
			
			for(i = 0; i < LEVEL_L_OPTION_MAX; i++)
			{
				temp_tok = FindValueToken(obj, LevelLocalOptionNames[i], g_str);
				if(!temp_tok)
				{
					temp_obj = FindObject(obj,  LevelLocalOptionNames[i]);
					if(!temp_obj)
					{
						continue;
					}
					temp_tok = (jsmntok_t*) temp_obj;
				}
				AddLocalOption(temp_ent,temp_tok, g_str, (level_local_option_t) i);
			}

			AddEntityToPhysics(temp_ent);
			AddCollisionHandlerToEntity(temp_ent);
			break;
		}
	case(LEVEL_L_OBJECT_OBJECT):
		{
			obj_name = FindValue(obj, LevelLocalObjectNames[LEVEL_L_OBJECT_OBJECT], g_str);
			if(!obj_name)
			{
				break;
			}
			cached_ent = FindCachedEntity(obj_name);
			if(!cached_ent)
			{
				break;
			}
			temp_ent = InitNewEntity();
			if(!temp_ent)
			{
				break;
			}
			memcpy(temp_ent, cached_ent, sizeof(entity_t));

			//Set Think Functions
			temp_ent->Think = NULL;
			temp_ent->Touch = NULL;
			temp_ent->Draw = DrawGeneric;
			temp_ent->mCollisionType = COLLISION_TYPE_STATIC;

			//Set Extra Options
			AddPhyicsToEntity(temp_ent);

			for(i = 0; i < LEVEL_L_OPTION_MAX; i++)
			{
				temp_tok = FindValueToken(obj, LevelLocalOptionNames[i], g_str);
				if(!temp_tok)
				{
					temp_obj = FindObject(obj,  LevelLocalOptionNames[i]);
					if(!temp_obj)
					{
						continue;
					}
					temp_tok = (jsmntok_t*) temp_obj;
				}
				AddLocalOption(temp_ent,temp_tok, g_str, (level_local_option_t) i);
			}

			AddEntityToPhysics(temp_ent);
			AddCollisionHandlerToEntity(temp_ent);

			break;
		}
	default:
		break;
	}
}

void AddGlobalOption(level_t* level, jsmntok_t* token, char* g_str, level_global_option_t type)
{
	switch(type)
	{
	case(LEVEL_G_OPTION_BACKGROUND):
		{
			level->mBackground = LoadSprite(JsmnToString(token, g_str), 0);
			break;
		}
	case(LEVEL_G_OPTION_HINT):
		{
			level->mHint = JsmnToString(token, g_str);
			break;
		}
	default:
		break;
	}
}

void AddLocalOption(entity_t* ent, void* token, char* g_str, level_local_option_t type)
{
	int i, count;
	vec2_t *temp_vec, pos_vec;
	cpVect cp_pos, cp_temp;
	object_t *temp_obj;
	jsmntok_t *temp_tok;
	ai_function_t *temp_ai;
	char *temp_str, *store_str,**temp_str_array;
	if(!ent || !token || !g_str)
	{
		return;
	}
	switch(type)
	{
	case(LEVEL_L_OPTION_POSITION):
		{
			temp_vec = ParseToVec2((object_t*)token, g_str);
			cp_temp = Vec2Cp(temp_vec);
			if(!temp_vec)
			{
				cpBodySetPos(ent->mPhysicsProperties->body, cpvzero);
				break;
			}
			cpBodySetPos(ent->mPhysicsProperties->body, cp_temp);
			if(temp_vec) free(temp_vec);
			break;
		}
	case(LEVEL_L_OPTION_TILE):
		{
			temp_vec = ParseToVec2((object_t*)token, g_str);
			if(!temp_vec)
			{
				break;
			}
			cp_pos = cpBodyGetPos(ent->mPhysicsProperties->body);
			cp_temp = cp_pos;
			pos_vec = CpToVect(&cp_temp);
			TileLevelEntity(ent, &pos_vec, temp_vec);
			if(temp_vec) free(temp_vec);
			break;
		}
	case(LEVEL_L_OPTION_VARIABLES):
		{
			temp_obj = (object_t*) token;
			count = CountMem(temp_obj->values, sizeof(jsmntok_t));
			temp_str_array = (char**) malloc(sizeof(char*)*(count+1));
			memset(temp_str_array, 0, sizeof(char*)*(count+1));
			for(i = 0; i < count; i++)
			{
				temp_str = JsmnToString(&temp_obj->values[i], g_str);
				if(!temp_str)
				{
					continue;
				}
				temp_str_array[i] = temp_str;
			}
			temp_str_array[count] = 0;
			//Easy way to save the position for next case statement
			ent->mCurrentFrame = (int) temp_str_array;
			break;
		}
	case(LEVEL_L_OPTION_AI):
		{
			temp_str = JsmnToString( (jsmntok_t*)token, g_str);
			if(!temp_str)
			{
				if(temp_str) free(temp_str);
				break;
			}
			ConvertFileToUseable(temp_str, NULL, &store_str, &temp_tok );
			if(!store_str || !temp_tok)
			{
				if(temp_str) free(temp_str);
				if(temp_tok) free(temp_tok);
				if(store_str) free(store_str);
				break;
			}
			temp_obj = ParseToObject(temp_tok, store_str);
			if(!temp_obj)
			{
				if(temp_str) free(temp_str);
				if(temp_tok) free(temp_tok);
				if(store_str) free(store_str);
				break;
			}
			temp_ai = ent->mCurrentFrame ? ParseAI(temp_obj, store_str, (char**)ent->mCurrentFrame) : ParseAI(temp_obj, store_str, NULL);
			if(ent->mCurrentFrame)
			{
				free((char**)ent->mCurrentFrame);
				ent->mCurrentFrame = 0;
			}
			if(!temp_ai)
			{
				if(temp_str) free(temp_str);
				if(temp_tok) free(temp_tok);
				if(store_str) free(store_str);
				break;
			}
			ent->mData = temp_ai;
			if(temp_str) free(temp_str);
			if(temp_tok) free(temp_tok);
			if(store_str) free(store_str);
			break;
		}
	case(LEVEL_L_OPTION_EXTRA):
		{
			temp_str = JsmnToString((jsmntok_t*)token, g_str);
			if(!temp_str)
			{
				break;
			}
			ent->Touch = TouchGoal;
			break;
		}
	default:
		break;
	}
}


