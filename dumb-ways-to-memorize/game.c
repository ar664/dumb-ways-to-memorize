#include "globals.h"
#include "game.h"
#include "entity.h"
#include "mystrings.h"
#include "parseobject.h"
#include "parsevariable.h"
#include "parselevel.h"
#include "graphics.h"
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//All char ** should be size+1, and ending member = NULL

int exitRequest = 0;
int gGravity = 9.8;
jsmn_parser gParser;
char **gLevels = NULL;  /**< The levels */
char **gSelectedLevels = NULL;  /**< The selected levels to load */
jsmntok_t *gGameTokens; /**< Tokens for GameData */
jsmntok_t *gEntityTokens;   /**< The entity tokens */
object_t *gGameObject;  /**< The game object */
object_t *gEntityObject;	/**< The entity object */
char *gGameData; /**< Game Data File */
char *gEntityData;  /**<Entity data file */
entity_t *gEntityDictionary; /**< Entities loaded from files AKA cached entities*/

/**
 * Loads game data from GameData.json, stored in gGameData.
 *
 * @return	state	0 on success, -1 on error.
 *
 * @author	Anthony Rios
 * @date	1/31/2016
 */

int LoadGameData()
{
	int num_tokens, i, lvlInt;
	char *lvlTest;
	jsmntok_t *lvlTokens;
	object_t *levelObj;
	//Init GameData Parse
	if(ConvertFileToUseable(JSON_FILE, &gParser, &gGameData, &gGameTokens) == -1)
	{
		printf("Failure to Parse GameData");
		return -1;
	}
	
	//Debug Checks & Object Parse

	gGameObject = ParseToObject(gGameTokens, gGameData);
	if(!gGameObject)
	{
		printf("Object parse error");
		return -1;
	}

	PrintObject(gGameObject, gGameData);

	levelObj = FindObject(gGameObject, "Levels");
	if(!levelObj)
	{
		printf("No levels found in gameObject");
		return -1;
	}
	lvlInt = CountMem(levelObj->values, sizeof(jsmntok_t));
	gLevels = (char**) malloc(sizeof(char*) * (lvlInt + 1));
	for(i = 0; i < lvlInt; i++)
	{
		gLevels[i] = JsmnToString(&levelObj->values[i], gGameData);
		if(!gLevels[i])
		{
			continue;
		}
		if( ConvertFileToUseable(gLevels[i], &gParser, &lvlTest, &lvlTokens) == -1)
		{
			printf("Level %s could not be loaded", gLevels[i]);
		}
		levelObj = ParseToObject(lvlTokens, lvlTest);
		PrintObject(levelObj, lvlTest);
	}

	return 0;
}

int LoadEntityData()
{
	int num_tokens, i, objects;
	entity_t *ent, *temp;

	//Init Entity Parse
	if( ConvertFileToUseable(JSON_FILE, &gParser, &gEntityData, &gEntityTokens) == -1)
	{
		printf("Failure to Parse Entity Data");
		return -1;
	}
	gEntityObject = ParseToObject(gEntityTokens, gEntityData);
	printf("Size of global tokens: %d \n", CountMem(gEntityTokens, sizeof(jsmntok_t)));
	Hazards_str = ParseToStringArray(FindObject(gEntityObject, "Hazards"), gEntityData);
	
	//Set Entity Dictionary
	objects = CountMem(gEntityObject->children, sizeof(object_t));
	gEntityDictionary = (entity_t*) malloc(sizeof(entity_t)*(objects+1));
	if(!gEntityDictionary)
	{
		printf("Alloc error lazy");
		return -1;
	}
	memset(gEntityDictionary, 0,sizeof(entity_t)*(objects+1));
	for(i = 0; i < objects; i++)
	{
		if(FindKey(gEntityObject->children[i].keys, "sprite(s)", gEntityData))
		{
			temp = ParseToEntity(&gEntityObject->children[i], gEntityData);
			if(!temp) continue;

			memcpy(&gEntityDictionary[i], temp, sizeof(entity_t));

			if(temp) free(temp);
		}
	}
	return 0;
}

/**
 * Select the levels randomly from the available levels, stores in gSelectedLevels.
 *
 * @return	0 on success, -1 on error
 *
 * @author	Anthony Rios
 * @date	1/31/2016
 */

int SelectLevels()
{
	int i, rand_i, *no_repeats, type_i;
	int levels = 0;
	i = 0;
	while(gLevels[i])
	{
		levels++; i++;
	}
	type_i = sizeof(int);
	no_repeats = (int*) malloc(sizeof(int)*(levels+1));
	memset(no_repeats, 0, sizeof(int));
	for (i = 0; i < LEVELS_PER_GAME; i++)
	{
		rand_i = rand()%levels;
		while(!CompareMemToMemArray(&rand_i, no_repeats, type_i, levels ))
		{
			rand_i = rand()%levels;
		}
		no_repeats[i] = rand_i;
		gSelectedLevels[i] = gLevels[rand_i];
	}
	gSelectedLevels[i+1] = NULL;
	free(no_repeats);
	return 0;
}

/**
 * Randomize selected levels.
 *
 * @author	Anthony Rios
 * @date	2/19/2016
 */

void RandomizeSelectedLevels()
{
	int i, rand_i, *no_repeats;
	char **slevel_copy;
	slevel_copy = (char**) malloc(sizeof(char*)*(LEVELS_PER_GAME+1));
	memcpy(slevel_copy, gSelectedLevels, sizeof(char*)*LEVELS_PER_GAME);
	slevel_copy[LEVELS_PER_GAME] = 0;
	no_repeats = (int*) malloc(sizeof(int)*LEVELS_PER_GAME+1);
	memset(no_repeats, 0, sizeof(int)*(LEVELS_PER_GAME+1));
	for(i = 0; gSelectedLevels[i]; i++)
	{
		rand_i = rand()%LEVELS_PER_GAME;
		while(!CompareMemToMemArray(&rand_i, no_repeats, sizeof(int), LEVELS_PER_GAME ))
		{
			rand_i = rand()%LEVELS_PER_GAME;
		}
		no_repeats[i] = rand_i;
		gSelectedLevels[i] = slevel_copy[rand_i];
	}
	free(no_repeats);
	free(slevel_copy);
}

int LoadSelectedLevel(int level)
{

}

void Poll()
{
	return;
}

void Update()
{
	return;
}

void Draw()
{
	return;
}

/**
 * Loads files and images for game.
 *
 * @return	state	returns 0 on success, -1 on error
 *
 * @author	Anthony Rios
 * @date	1/31/2016
 */

int Setup()
{
	sprite_t *test_sprite;
	vec2_t test_vec = {0,0};
	srand(SDL_GetTicks());
	//atexit(Shutdown);

	if(LoadGameData())
	{
		perror("Loading game data went wrong");
		return -1;
	}
	if(InitGraphics())
	{
		perror("Initializing entity system went wrong");
		return -1;
	}
	if(InitEntitySystem())
	{
		perror("Initializing entity system went wrong");
		return -1;
	}
	if(LoadEntityData())
	{
		perror("Loading entity data went wrong");
		return -1;
	}

	test_sprite = LoadSprite("Sprite/UI/NESController.png",0);
	test_sprite->mCurrentFrame = LoadAnimation(test_sprite->mSize.x, test_sprite->mSize.y, test_sprite->mSize.x, test_sprite->mSize.y);
	/*
	while(SDL_GetTicks()>210000)
	{
		DrawSprite(test_sprite, &test_vec, gRenderer);
		SDL_RenderPresent(gRenderer);
		SDL_Delay(17);
		SDL_RenderClear(gRenderer);
	}
	*/
	FreeSprite(test_sprite);

	return 0;
}

int Run()
{
	while(!exitRequest)
	{
		Poll();
		Update();
		Draw();
	}
	return 0;
}

void Shutdown()
{

	return;
}
	