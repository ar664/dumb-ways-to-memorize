#include "globals.h"
#include "game.h"
#include "mystrings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int exitRequest = 0;
jsmn_parser gParser;
jsmntok_t *gTokens;

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
	int num_tokens;
	FILE *file;
	char *filebuffer;
	file = fopen(JSON_FILE, "r");
	filebuffer = FileToString(file);
	jsmn_init(&gParser);
	num_tokens = jsmn_parse(&gParser, filebuffer, strlen(filebuffer), NULL, 0);
	gTokens = (jsmntok_t*) malloc(sizeof(jsmntok_t)*num_tokens);
	jsmn_parse(&gParser, filebuffer, strlen(filebuffer), gTokens, num_tokens);
	if(!gTokens)
	{
		printf("JSON parse error");
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
}

int Shutdown()
{
	return 0;
}