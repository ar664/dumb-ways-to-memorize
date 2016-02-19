#include "globals.h"
#include "game.h"
#include "entity.h"
#include "mystrings.h"
#include "parseobject.h"
#include "parsevariable.h"
#include "graphics.h"
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//All char ** should be size+1, and ending member = NULL

int exitRequest = 0;
jsmn_parser gParser;
char **gLevels = NULL;  /**< The levels */
char **gSelectedLevels = NULL;  /**< The selected levels to load */
jsmntok_t *gGameTokens; /**< Tokens for GameData */
jsmntok_t *gEntityTokens;
object_t *gGameObject;
object_t *gEntityObject;
char *gGameData; /**< Game Data File */
char *gEntityData;

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
	int num_tokens, i;
	gGameData = FileToString(JSON_FILE);
	jsmn_init(&gParser);
	num_tokens = jsmn_parse(&gParser, gGameData, strlen(gGameData), NULL, 0);
	gGameTokens = (jsmntok_t*) malloc(sizeof(jsmntok_t)*(num_tokens+1));
	jsmn_init(&gParser); //Reset parser
	num_tokens = jsmn_parse(&gParser, gGameData, strlen(gGameData), gGameTokens, num_tokens);
	if(!num_tokens)
	{
		printf("JSON parse error");
		return -1;
	}
	memset(&gGameTokens[num_tokens+1], 0, sizeof(jsmntok_t)); 
	for(i = 0; i < num_tokens; i++)
	{
		printf("JSON token %d : %s size: %d\n", i, TypeFromJSON(gGameTokens[i].type), gGameTokens[i].size);
	}

	gGameObject = ParseToObject(gGameTokens, gGameData);
	printf("Size of global tokens: %d", CountMem(gGameTokens, sizeof(jsmntok_t)));
	return 0;
}

int LoadEntityData()
{
	int num_tokens, i, objects;
	entity_t *ent, *temp;
	gEntityData = FileToString(ENTITY_FILE);
	jsmn_init(&gParser);
	num_tokens = jsmn_parse(&gParser, gEntityData, strlen(gEntityData), NULL, 0);
	gEntityTokens = (jsmntok_t*) malloc(sizeof(jsmntok_t)*(num_tokens+1));
	jsmn_init(&gParser); //Reset parser
	num_tokens = jsmn_parse(&gParser, gEntityData, strlen(gEntityData), gEntityTokens, num_tokens);
	if(!num_tokens)
	{
		printf("JSON parse error");
		return -1;
	}
	memset(&gEntityTokens[num_tokens+1], 0, sizeof(jsmntok_t)); 
	gEntityObject = ParseToObject(gEntityTokens, gEntityData);
	printf("Size of global tokens: %d", CountMem(gEntityTokens, sizeof(jsmntok_t)));
	Hazards_str = ParseToStringArray(FindObject(gEntityObject, "Hazards"), gEntityData);
	//ent = ParseToEntity(FindObject(gEntityObject, "Entity Ex"), gEntityData);
	objects = CountMem(gEntityObject->children, sizeof(object_t));
	for(i = 0; i < objects; i++)
	{
		if(FindKey(gEntityObject->children[i].keys, "sprite(s)", gEntityData))
		{
			ent = InitNewEntity();
			temp = ParseToEntity(&gEntityObject->children[i], gEntityData);
			if(!temp) continue;
			if(!ent) break;
			memcpy(ent, temp, sizeof(entity_t));
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
	no_repeats = (int*) calloc(levels,sizeof(int));
	for (i = 0; i < LEVELS_PER_GAME; i++)
	{
		rand_i = rand()%levels;
		while(!CompareMemToMemArray(&rand_i, no_repeats, type_i, levels ))
		{
			rand_i = rand()%levels;
		}
		gSelectedLevels[i] = gLevels[rand_i];
	}
	gSelectedLevels[i+1] = NULL;
	return 0;
}

int LoadSelectedLevels()
{
	return 0;
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
	