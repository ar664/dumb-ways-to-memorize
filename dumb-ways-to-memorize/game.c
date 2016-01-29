#include "globals.h"
#include "game.h"
#include "mystrings.h"
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int exitRequest = 0;
jsmn_parser gParser;
jsmntok_t *gTokens; /**< Tokens for GameData */
char *gGameData; /**< Game Data File */

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
		printf("JSON token %d : %s \n", i, TypeFromJSON(gTokens[i].type));
		printf("JSON token size %d : %d \n", i, gTokens[i].size);
	}
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

int Setup()
{
	if(LoadGameData())
	{
		perror("Loading game data went wrong");
		return -1;
	}
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

int Shutdown()
{
	return 0;
}