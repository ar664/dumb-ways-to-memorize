#include "globals.h"
#include "game.h"
#include "mystrings.h"
#include "parseobject.h"
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
jsmntok_t *gTokens; /**< Tokens for GameData */
object_t *gGameObject;
char *gGameData; /**< Game Data File */

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
	gTokens = (jsmntok_t*) malloc(sizeof(jsmntok_t)*(num_tokens+1));
	jsmn_init(&gParser); //Reset parser
	num_tokens = jsmn_parse(&gParser, gGameData, strlen(gGameData), gTokens, num_tokens);
	if(!num_tokens)
	{
		printf("JSON parse error");
		return -1;
	}
	memset(&gTokens[num_tokens+1], 0, sizeof(jsmntok_t)); 
	for(i = 0; i < num_tokens; i++)
	{
		printf("JSON token %d : %s size: %d\n", i, TypeFromJSON(gTokens[i].type), gTokens[i].size);
	}

	gGameObject = ParseToObject(gTokens, gGameData);
	printf("Size of global tokens: %d", CountMem(gTokens, sizeof(jsmntok_t)));

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
	srand(SDL_GetTicks());
	atexit(Shutdown);

	if(LoadGameData())
	{
		perror("Loading game data went wrong");
		return -1;
	}

	InitGraphics();
	
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
	SDL_Quit();
	while(gSpriteNum)
	{
		FreeSprite(&gSprites[gSpriteNum-1]);
	}

	return;
}
