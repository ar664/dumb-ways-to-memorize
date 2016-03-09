#include "entity.h"
#include "parselevel.h"
#include "parseobject.h"
#include "parsevariable.h"
#include "mystrings.h"
#include <stdio.h>

level_t *gCurrentLevel = NULL;

//Occurs after entity loads entity list
int LoadLevel(object_t *level, char *g_str)
{
	jsmntok_t *tempTok;
	object_t *tempObj, *enemyObj, *posObj;
	entity_t *tempEnt, *cachedEnt;
	int tempInt, i, j, enemies, positions;
	vec2_t *spawn;
	if(!level || !level->keys)
	{
		printf("Could not load NULL lvl");
		return -1;
	}
	if(!gCurrentLevel)
	{
		gCurrentLevel = (level_t*) malloc(sizeof(level_t));
	} else {
		memset(gCurrentLevel, 0, sizeof(level_t));
	}
	
	//Assign Name & Hint
	tempTok = FindKey(level->keys, "Name", g_str);
	if(!tempTok)
	{
		printf("Level %s has no Name", level->name);
		return -1;
	}
	tempInt = abs((int) level->keys - (int) tempTok);
	gCurrentLevel->mName = JsmnToString(&level->values[tempInt], g_str);

	if( (tempTok = FindKey(level->keys, "Hint", g_str)) != NULL)
	{
		tempInt = abs((int) level->keys - (int) tempTok);
		gCurrentLevel->mHint = JsmnToString(&level->values[tempInt], g_str);
	}
	
	//Assign Background
	tempTok = FindKey(level->keys, "Background", g_str);
	if(!tempTok)
	{
		printf("Level %s has no Background", level->name);
		return -1;
	}
	tempInt = abs((int) level->keys - (int) tempTok);
	gCurrentLevel->mBackground = LoadSprite(JsmnToString(&level->values[tempInt], g_str), 0);

	//Assign Spawn Point
	tempObj = FindObject(level->children, "Spawn");
	if(!tempObj)
	{
		printf("Level %s has no Spawn", level->name);
		return -1;
	}
	spawn = ParseToVec2(tempObj, g_str);
	gCurrentLevel->mSpawnPoint = *spawn;
	free(spawn);

	//Spawn Enemies
	enemyObj = FindObject(level->children, "Enemies");
	if(enemyObj && enemyObj->children)
	{
		enemies = CountMem(enemyObj->children, sizeof(object_t));
		for(i = 0; i < enemies; i++)
		{
			tempTok = FindKey(enemyObj->children[i].keys, "enemy", g_str);
			if(!tempTok)
			{
				continue;
			}
			tempInt = (int) enemyObj->children[i].keys - (int) tempTok;
			posObj = FindObject(enemyObj, "position");
			if(!posObj)
			{
				continue;
			}
			if(posObj->children)
			{
				positions = CountMem(posObj->children, sizeof(object_t));
				for(j = 0; j < positions; j++)
				{
					tempEnt = InitNewEntity();
					if(!tempEnt)
					{
						continue;
					}
					cachedEnt = FindCachedEntity(JsmnToString(&level->values[tempInt], g_str));
					if(!cachedEnt)
					{
						continue;
					}
					memcpy(tempEnt, cachedEnt , sizeof(entity_t));
					tempEnt->mPosition = *ParseToVec2(&posObj->children[j], g_str);
				}
			} else
			{
				tempEnt = InitNewEntity();
				if(!tempEnt)
				{
					continue;
				}
				cachedEnt = FindCachedEntity(JsmnToString(&level->values[tempInt], g_str));
				if(!cachedEnt)
				{
					continue;
				}
				memcpy(tempEnt, cachedEnt, sizeof(entity_t));
				tempEnt->mPosition = *ParseToVec2(posObj, g_str);
			}
		}
	}
	
	//Spawn Objects - same as enemies for now
	enemyObj = FindObject(level->children, "Objects");
	if(enemyObj && enemyObj->children)
	{
		enemies = CountMem(enemyObj->children, sizeof(object_t));
		for(i = 0; i < enemies; i++)
		{
			tempTok = FindKey(enemyObj->children[i].keys, "object", g_str);
			if(!tempTok)
			{
				continue;
			}
			tempInt = (int) enemyObj->children[i].keys - (int) tempTok;
			posObj = FindObject(enemyObj, "position");
			if(!posObj)
			{
				continue;
			}
			if(posObj->children)
			{
				positions = CountMem(posObj->children, sizeof(object_t));
				for(j = 0; j < positions; j++)
				{
					tempEnt = InitNewEntity();
					if(!tempEnt)
					{
						continue;
					}
					cachedEnt = FindCachedEntity(JsmnToString(&level->values[tempInt], g_str));
					if(!cachedEnt)
					{
						continue;
					}
					memcpy(tempEnt, cachedEnt, sizeof(entity_t));
					tempEnt->mPosition = *ParseToVec2(&posObj->children[j], g_str);
				}
			} else
			{
				tempEnt = InitNewEntity();
				if(!tempEnt)
				{
					continue;
				}
				cachedEnt = FindCachedEntity(JsmnToString(&level->values[tempInt], g_str));
				if(!cachedEnt)
				{
					continue;
				}
				memcpy(tempEnt, cachedEnt, sizeof(entity_t));
				tempEnt->mPosition = *ParseToVec2(posObj, g_str);
			}
		}
	}

	return 0;
}


void DrawLevel()
{
	if(!gCurrentLevel)
	{
		printf("Level not loaded, unable to print");
		return;
	}
	DrawSprite(gCurrentLevel->mBackground, NULL , gRenderer);
}