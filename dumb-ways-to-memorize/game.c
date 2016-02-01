#include "globals.h"
#include "game.h"
#include "mystrings.h"
#include "parseobject.h"
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
	gTokens = (jsmntok_t*) malloc(sizeof(jsmntok_t)*num_tokens);
	jsmn_init(&gParser); //Reset parser
	num_tokens = jsmn_parse(&gParser, gGameData, strlen(gGameData), gTokens, num_tokens);
	if(!num_tokens)
	{
		printf("JSON parse error");
		return -1;
	}

	for(i = 0; i < num_tokens; i++)
	{
		printf("JSON token %d : %s size: %d\n", i, TypeFromJSON(gTokens[i].type), gTokens[i].size);
	}

	//printf("First Level: %s",FindValueFromKey(gTokens, "Level", gGameData ));
	printf("Size of char: %d \n", sizeof(char));
	printf("Size of int: %d \n", sizeof(int));
	printf("Size of jsmntok_t: %d \n", sizeof(jsmntok_t));
	printf("Size of object_t: %d \n", sizeof(object_t));

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
	if(LoadGameData())
	{
		perror("Loading game data went wrong");
		return -1;
	}
	atexit(SDL_Quit);
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
	return Shutdown();
}

int Shutdown()
{
	return 0;
}