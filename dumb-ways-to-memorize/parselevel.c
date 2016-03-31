#include "entity.h"
#include "parselevel.h"
#include "parseobject.h"
#include "parseentity.h"
#include "ai_interpret.h"
#include "mystrings.h"
#include <stdio.h>

level_t *gCurrentLevel = NULL;  

int LoadLevel(object_t *level, char *g_str)
{
	jsmntok_t *tempTok, *aiTok, *enemyTok;
	object_t *tempObj, *enemyObj, *posObj, *itemObj, *aiObj;
	entity_t *tempEnt, *cachedEnt;
	ai_function_t *enemyAI;
	int tempInt, i, j, x, y, enemies, objects, positions, tileX, tileY;
	char *temp_str  = NULL, *aiStr, *enemyName, *objectName, *aiFile, *tileX_str, *tileY_str;
	vec2_t *spawn, *temp_pos, obj_pos, tile_pos;
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
			
			enemyName = FindValue(&enemyObj->children[i], LEVEL_ENEMY_NAME_STR, g_str);
			if(!enemyName)
			{
				continue;
			}
			cachedEnt = FindCachedEntity(enemyName);
			if(!cachedEnt)
			{
				FreeEntity(cachedEnt);
				continue;
			}

			aiFile = FindValue(&enemyObj->children[i], LEVEL_AI_STR, g_str);
			if(!aiFile)
			{
				enemyAI = NULL;
			} else
			{
				//Find and parse AI
				ConvertFileToUseable(aiFile, NULL, &aiStr,&aiTok );
				aiObj = ParseToObject(aiTok, aiStr);
				if( (tempObj = FindObject(aiObj, LEVEL_VARIABLES_STR)) != NULL )
				{
					enemyAI = ParseAI(aiObj, aiStr, ParseToStringArray(tempObj, aiStr));
				} else
				{
					enemyAI = ParsePresetAI(aiObj, aiStr);
				}
				
			}
			posObj = FindObject(&enemyObj->children[i], LEVEL_POSITION_STR);
			tileX_str = FindValue(&enemyObj->children[i], LEVEL_TILE_X_STR, g_str);
			tileY_str = FindValue(&enemyObj->children[i], LEVEL_TILE_Y_STR, g_str);
			tileX = StrToInt(tileX_str);
			tileY = StrToInt(tileY_str);
			if(!posObj)
			{
				continue;
			}
			

			positions = posObj->children ? CountMem(posObj->children, sizeof(object_t)) : 1;

			for(j = 0; j < positions; j++)
			{
				temp_pos = ParseToVec2(posObj->children ? &posObj->children[j] : posObj, g_str);
				if(!temp_pos) 
					continue;
				obj_pos.x = tileX ? (tileX - temp_pos->x)/cachedEnt->mSprites[0]->mSize.x : 1;
				obj_pos.y = tileY ? (tileY - temp_pos->y)/cachedEnt->mSprites[0]->mSize.y : 1;
				for(x = 0; x < obj_pos.x; x++)
				{
					for(y = 0; y < obj_pos.y; y++)
					{
						tempEnt = InitNewEntity();
						if(!tempEnt)
						{
							printf("Max Entities reached for level %s", level->name);
							continue;
						}
						
						memcpy(tempEnt, cachedEnt , sizeof(entity_t));
						
						tile_pos.x = tileX ? temp_pos->x + x*cachedEnt->mSprites[0]->mSize.x: temp_pos->x;
						tile_pos.y = tileY ? temp_pos->y + y*cachedEnt->mSprites[0]->mSize.y: temp_pos->y;
						tempEnt->mPosition = tile_pos;
						tempEnt->mData = enemyAI;
						tempEnt->Think = ThinkEnemy;
						tempEnt->Draw = DrawGeneric;
						tempEnt->Touch = TouchGeneric;
						tempEnt->mWeight = 1;
						tempEnt->mNextThink = SDL_GetTicks() + 10;
						tile_pos.x = 0; tile_pos.y = 0;
					}
				}
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
		objects = CountMem(itemObj->children, sizeof(object_t));
		for(i = 0; i < objects; i++)
		{
			objectName = FindValue(&itemObj->children[i], LEVEL_ITEM_NAME_STR, g_str);
			if(!objectName)
			{
				continue;
			}

			cachedEnt = FindCachedEntity(objectName);
			if(!cachedEnt)
			{
				FreeEntity(cachedEnt);
				continue;
			}

			posObj = FindObject(&itemObj->children[i], LEVEL_POSITION_STR);
			tileX_str = FindValue(itemObj, LEVEL_TILE_X_STR, g_str);
			tileY_str = FindValue(itemObj, LEVEL_TILE_Y_STR, g_str);
			tileX = StrToInt(tileX_str);
			tileY = StrToInt(tileY_str);
			if(!posObj)
			{
				continue;
			}
			obj_pos.x = tileX ? (tileX - 0)/cachedEnt->mSprites[0]->mSize.x : 1;
			obj_pos.y = tileY ? (tileY - 0)/cachedEnt->mSprites[0]->mSize.y : 1;
			
			positions = posObj->children ? CountMem(posObj->children, sizeof(object_t)) : 1;

			for(j = 0; j < positions; j++)
			{
				temp_pos = ParseToVec2(posObj->children ? &posObj->children[j] : posObj, g_str);
				if(!temp_pos) 
					continue;
				obj_pos.x = tileX ? (tileX - temp_pos->x)/cachedEnt->mSprites[0]->mSize.x : 1;
				obj_pos.y = tileY ? (tileY - temp_pos->y)/cachedEnt->mSprites[0]->mSize.y : 1;
				for(x = 0; x < obj_pos.x; x++)
				{
					for(y = 0; y < obj_pos.y; y++)
					{
						tempEnt = InitNewEntity();
						if(!tempEnt)
						{
							printf("Max Entities reached in level : %s \n", level->name);
							continue;
						}
						
						memcpy(tempEnt, cachedEnt, sizeof(entity_t));
						tempEnt->Draw = DrawGeneric;
						tempEnt->Think = NULL;
						tempEnt->Touch = FindValue(&itemObj->children[i], LEVEL_ITEM_XTRA_STR, g_str) ? TouchGoal : NULL;
						tempEnt->mCollisionType = COLLISION_TYPE_STATIC;
						tile_pos.x = tileX ? temp_pos->x + x*cachedEnt->mSprites[0]->mSize.x: temp_pos->x;
						tile_pos.y = tileY ? temp_pos->y + y*cachedEnt->mSprites[0]->mSize.y: temp_pos->y;
						tempEnt->mPosition = tile_pos;
						tile_pos.x = 0; tile_pos.y = 0;
					}
				}
			}
		}
	}

	return 0;
}

void SpawnInLevel();


void DrawLevel()
{
	if(!gCurrentLevel)
	{
		printf("Level not loaded, unable to print");
		return;
	}
	DrawSprite(gCurrentLevel->mBackground, NULL , NULL, gRenderer);
}