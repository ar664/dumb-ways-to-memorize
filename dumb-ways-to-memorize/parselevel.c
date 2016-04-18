#include "entity.h"
#include "parselevel.h"
#include "parseobject.h"
#include "parseentity.h"
#include "ai_interpret.h"
#include "mystrings.h"
#include "dumb_physics.h"
#include <stdio.h>

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
			cpPos.x = new_pos.x;
			cpPos.y = new_pos.y;
			AddPhyicsToEntity(temp_ent);
			if(!temp_ent->mPhysicsProperties)
			{
				return;
			}
			cpBodySetPos(temp_ent->mPhysicsProperties->body, cpPos);
			new_pos.x = 0; new_pos.y = 0;
			AddEntityToPhysics(temp_ent);
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

void DrawLevel()
{
	if(!gCurrentLevel)
	{
		printf("Level not loaded, unable to print");
		return;
	}
	DrawSprite(gCurrentLevel->mBackground, NULL , NULL, gRenderer);
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
	vec2_t *temp_vec, *pos_vec;
	cpVect cp_pos, *cp_temp;
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
			cp_temp = (cpVect*) Vec2Cp(temp_vec);
			if(!temp_vec || !cp_temp)
			{
				cpBodySetPos(ent->mPhysicsProperties->body, cpvzero);
				break;
			}
			cpBodySetPos(ent->mPhysicsProperties->body, *cp_temp);
			if(temp_vec) free(temp_vec);
			if(cp_temp) free(cp_temp);
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
			cp_temp = &cp_pos;
			pos_vec = CpToVect(cp_temp);
			TileLevelEntity(ent, pos_vec, temp_vec);
			if(temp_vec) free(temp_vec);
			if(pos_vec) free(pos_vec);
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
				temp_str = JsmnToString(temp_obj->values, g_str);
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
				break;
			}
			ConvertFileToUseable(temp_str, NULL, &store_str, &temp_tok );
			if(!store_str || !temp_tok)
			{
				break;
			}
			temp_obj = ParseToObject(temp_tok, store_str);
			if(!temp_obj)
			{
				break;
			}
			temp_ai = ent->mCurrentFrame ? ParseAI(temp_obj, store_str, (char**)ent->mCurrentFrame) : ParsePresetAI(temp_obj, store_str);
			if(!temp_ai)
			{
				break;
			}
			ent->mData = temp_ai;
			ent->mCurrentFrame = 0;
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


