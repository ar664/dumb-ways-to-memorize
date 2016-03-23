#include "entity.h"
#include "parselevel.h"
#include "parseobject.h"
#include "parsevariable.h"
#include "ai_interpret.h"
#include "mystrings.h"
#include <stdio.h>

level_t *gCurrentLevel = NULL;

//Occurs after entity loads entity list
int LoadLevel(object_t *level, char *g_str)
{
	jsmntok_t *tempTok, *aiTok, *enemyTok;
	object_t *tempObj, *enemyObj, *posObj, *itemObj, *aiObj;
	entity_t *tempEnt, *cachedEnt;
	ai_function_t *enemyAI;
	int tempInt, i, j, enemies, positions;
	char *temp_str  = NULL, *aiStr, *enemyName, *objectName;
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
	tempTok = FindKey(level->keys, G_NAME_STR, g_str);
	if(!tempTok)
	{
		printf("Level %s has no Name \n", level->name);
		return -1;
	}
	tempInt = tempTok - level->keys;
	gCurrentLevel->mName = JsmnToString(&level->values[tempInt], g_str);

	if( (tempTok = FindKey(level->keys, "Hint", g_str)) != NULL)
	{
		tempInt = tempTok - level->keys;
		gCurrentLevel->mHint = JsmnToString(&level->values[tempInt], g_str);
	}
	
	//Assign Background
	tempTok = FindKey(level->keys, LEVEL_BACKGROUND_STR, g_str);
	if(!tempTok)
	{
		printf("Level %s has no Background \n", gCurrentLevel->mName);
		return -1;
	}
	tempInt = tempTok - level->keys;
	gCurrentLevel->mBackground = LoadSprite(JsmnToString(&level->values[tempInt], g_str), 0);

	//Assign Spawn Point
	tempObj = FindObject(level->children, LEVEL_SPAWN_STR);
	if(!tempObj)
	{
		printf("Level %s has no Spawn \n", gCurrentLevel->mName);
		return -1;
	}
	spawn = ParseToVec2(tempObj, g_str);
	if(!spawn)
	{
		printf("Level %s spawn parse error \n", gCurrentLevel->mName);
		return -1;
	}
	gCurrentLevel->mSpawnPoint = *spawn;
	free(spawn);

	//Spawn Enemies
	enemyObj = FindObject(level, LEVEL_ENEMY_OBJ_STR);
	if(!enemyObj)
	{
		printf("Level failed to find enemies in level : %s \n", level->name);
		return -1;
	}
	if(enemyObj && enemyObj->children)
	{
		enemies = CountMem(enemyObj->children, sizeof(object_t));
		for(i = 0; i < enemies; i++)
		{
			enemyTok = FindKey(enemyObj->children[i].keys, LEVEL_ENEMY_NAME_STR, g_str);
			tempInt = enemyTok - enemyObj->children[i].keys;
			enemyName = JsmnToString(&enemyObj->children[i].values[tempInt], g_str);
			if(!enemyTok)
			{
				continue;
			}
			
			tempTok = FindKey(enemyObj->children[i].keys, LEVEL_AI_STR, g_str);
			tempInt = tempTok - enemyObj->children[i].keys;
			if(!tempTok)
			{
				enemyAI = NULL;
			} else
			{
				//Find and parse AI
				ConvertFileToUseable(JsmnToString(&enemyObj->children[i].values[tempInt], g_str), NULL, &aiStr,&aiTok );
				aiObj = ParseToObject(aiTok, aiStr);
				if( (tempObj = FindObject(aiObj, LEVEL_VARIABLES_STR)) != NULL )
				{
					enemyAI = ParseAI(aiObj, aiStr, ParseToStringArray(tempObj, aiStr));
				} else
				{
					enemyAI = ParsePresetAI(aiObj, aiStr);
				}
				
			}
			posObj = FindObject(enemyObj, LEVEL_POSITION_STR);
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
						FreeEntity(tempEnt);
						continue;
					}
					cachedEnt = FindCachedEntity(enemyName);
					if(!cachedEnt)
					{
						FreeEntity(tempEnt);
						continue;
					}
					memcpy(tempEnt, cachedEnt , sizeof(entity_t));
					tempEnt->mPosition = *ParseToVec2(&posObj->children[j], g_str);
					tempEnt->mData = enemyAI;
					tempEnt->Think = ThinkEnemy;
					tempEnt->Draw = DrawGeneric;
					tempEnt->Touch = TouchEnemy;
					tempEnt->mWeight = 1;
					tempEnt->mNextThink = SDL_GetTicks() + 10;
				}
			} else
			{
				tempEnt = InitNewEntity();
				if(!tempEnt)
				{
					FreeEntity(tempEnt);
					continue;
				}

				cachedEnt = FindCachedEntity(enemyName);
				if(!cachedEnt)
				{
					FreeEntity(tempEnt);
					continue;
				}
				memcpy(tempEnt, cachedEnt, sizeof(entity_t));
				tempEnt->mPosition = *ParseToVec2(posObj, g_str);
				tempEnt->mData = enemyAI;
				tempEnt->Think = ThinkEnemy;
				tempEnt->Draw = DrawGeneric;
				tempEnt->Touch = TouchEnemy;
				tempEnt->mWeight = 1;
				tempEnt->mNextThink = SDL_GetTicks() + 10;
			}
			if(temp_str) free(temp_str);
			temp_str = NULL;
		}
	}
	
	//Spawn Objects
	itemObj = FindObject(level, LEVEL_ITEM_OBJ_STR);
	if(!itemObj)
	{
		printf("Level failed to find objects in level : %s \n", level->name);
		return -1;
	}
	if(itemObj && itemObj->children)
	{
		enemies = CountMem(itemObj->children, sizeof(object_t));
		for(i = 0; i < enemies; i++)
		{
			tempTok = FindKey(itemObj->children[i].keys, LEVEL_ITEM_NAME_STR, g_str);
			if(!tempTok)
			{
				continue;
			}
			tempInt = tempTok - itemObj->children[i].keys ;
			objectName = JsmnToString(&itemObj->children[i].values[tempInt], g_str);

			posObj = FindObject(itemObj, LEVEL_POSITION_STR);
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
						FreeEntity(tempEnt);
						continue;
					}

					cachedEnt = FindCachedEntity(objectName);
					if(!cachedEnt)
					{
						FreeEntity(tempEnt);
						continue;
					}
					memcpy(tempEnt, cachedEnt, sizeof(entity_t));
					tempEnt->Draw = DrawGeneric;
					tempEnt->Think = NULL;
					tempEnt->Touch = NULL;
					tempEnt->mCollisionType = COLLISION_TYPE_STATIC;
					tempEnt->mPosition = *ParseToVec2(&posObj->children[j], g_str);
				}
			} else
			{
				tempEnt = InitNewEntity();
				if(!tempEnt)
				{
					FreeEntity(tempEnt);
					continue;
				}

				cachedEnt = FindCachedEntity(objectName);
				if(!cachedEnt)
				{
					FreeEntity(tempEnt);
					continue;
				}
				memcpy(tempEnt, cachedEnt, sizeof(entity_t));
				tempEnt->mPosition = *ParseToVec2(posObj, g_str);
				tempEnt->Draw = DrawGeneric;
				tempEnt->Think = NULL;
				tempEnt->Touch = NULL;
				tempEnt->mCollisionType = COLLISION_TYPE_STATIC;
			}
			if(temp_str) free(temp_str);
			temp_str = NULL;
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
	DrawSprite(gCurrentLevel->mBackground, NULL , NULL, gRenderer);
}