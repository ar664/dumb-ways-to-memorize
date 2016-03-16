#include "globals.h"
#include "game.h"
#include "entity.h"
#include "mystrings.h"
#include "parseobject.h"
#include "parsevariable.h"
#include "parsepowerup.h"
#include "parselevel.h"
#include "graphics.h"
#include <SDL.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"

//All char ** should be size+1, and ending member = NULL

int exitRequest = 0;
int gLives = 0;
int gLevelsPerGame = LEVELS_DEFAULT;
float gGravity = 9.8;
jsmn_parser gParser;
char **gLevels = NULL;  /**< The levels */
char **gSelectedLevels = NULL;  /**< The selected levels to load */
char **gSelectedPowerUps = NULL; /**< The power ups the player selects */
jsmntok_t *gGameTokens; /**< Tokens for GameData */
jsmntok_t *gEntityTokens;   /**< The entity tokens */
jsmntok_t *gLevelTokens;
object_t *gGameObject;  /**< The game object */
object_t *gEntityObject;	/**< The entity object */
object_t *gLevelObject;
char *gGameData; /**< Game Data File */
char *gEntityData;  /**<Entity data file */
char *gLevelData;
entity_t *gEntityDictionary; /**< Entities loaded from files AKA cached entities*/
power_t *gPowerUps;
GameState gGameState = SPLASH;
sprite_t *gSplash = NULL;
vec2_t ZeroPos = {0,0};
SDL_Event gEventQ;
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

	return 0;
}

int LoadEntityData()
{
	int i, objects;
	entity_t *temp;

	//Init Entity Parse
	if( ConvertFileToUseable(ENTITY_FILE, &gParser, &gEntityData, &gEntityTokens) == -1)
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

int LoadLevelData()
{
	object_t *levelObj;
	int i, lvlInt;

	//Find Levels
	levelObj = FindObject(gGameObject, "Levels");
	gLevelsPerGame = FindKey(gGameTokens, LEVELS_NOT_DEFAULT, gGameData) ? -1 : LEVELS_DEFAULT;
	if(gLevelsPerGame == -1)
	{
		JsmnToInt(FindKey(gGameTokens, LEVELS_NOT_DEFAULT, gGameData)+1, gGameData, &gLevelsPerGame);
		if(!gLevelsPerGame)
			gLevelsPerGame = LEVELS_DEFAULT;
	}
	if(!levelObj)
	{
		printf("No levels found in gameObject");
		return -1;
	}

	//Alloc
	lvlInt = CountMem(levelObj->values, sizeof(jsmntok_t));
	gLevels = (char**) malloc(sizeof(char*) * (lvlInt + 1));

	//Load
	for(i = 0; i < lvlInt; i++)
	{
		gLevels[i] = JsmnToString(&levelObj->values[i], gGameData);
		if(!gLevels[i])
		{
			continue;
		}
		printf("Level : %s \n", gLevels[i]);
	}

	gLevels[lvlInt] = NULL;
	return 0;
}

//After Load GameData, Before Menu
int LoadPowerUpData()
{
	object_t *powers;
	int powerCount, i;

	powers = FindObject(gGameObject, "PowerUps");
	if(!powers)
	{
		printf("Could not find powerups in GameData");
		return -1;
	}

	powerCount = CountMem(powers->children, sizeof(object_t*));
	gPowerUps = (power_t*) malloc(sizeof(power_t)*( powerCount+1 ));
	if(!gPowerUps)
	{
		printf("Could not allocate power ups");
		return -1;
	}

	for(i = 0; i < powerCount; i++)
	{
		gPowerUps[i] = *ParseToPowerUp(&powers->children[i], gGameData);
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

	//Count 
	while(gLevels[levels])
	{
		levels++;
	}
	type_i = sizeof(int);

	//Alloc
	gSelectedLevels = (char**) malloc(sizeof(char*)*levels+1);
	no_repeats = (int*) malloc(sizeof(int)*(levels+1));
	if(!gSelectedLevels) return -1;
	if(!no_repeats) return -1;
	
	//Select
	memset(no_repeats, 0, sizeof(int));
	for (i = 0; i < gLevelsPerGame; i++)
	{
		rand_i = rand()%levels;
		if(i >= levels)
		{
			break;
		}
		while(!CompareMemToMemArray(&rand_i, no_repeats, type_i, levels ))
		{
			rand_i = rand()%levels;
		}
		no_repeats[i] = rand_i;
		gSelectedLevels[i] = gLevels[rand_i];
	}
	gSelectedLevels[i] = NULL;

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

	if (CountMem(gLevels, sizeof(char*)) < gLevelsPerGame)
	{
		printf("Levels less than min, Easy Mode - No Rand");
		return;
	}

	//Get a copy of gSelected
	slevel_copy = (char**) malloc(sizeof(char*)*(gLevelsPerGame+1));
	memcpy(slevel_copy, gSelectedLevels, sizeof(char*)*gLevelsPerGame);
	slevel_copy[gLevelsPerGame] = 0;

	//No Repeats
	no_repeats = (int*) malloc(sizeof(int)*gLevelsPerGame+1);
	memset(no_repeats, 0, sizeof(int)*(gLevelsPerGame+1));

	//Randomize
	for(i = 0; gSelectedLevels[i]; i++)
	{
		rand_i = rand()%gLevelsPerGame;
		while(!CompareMemToMemArray(&rand_i, no_repeats, sizeof(int), gLevelsPerGame ))
		{
			rand_i = rand()%gLevelsPerGame;
		}
		no_repeats[i] = rand_i;
		gSelectedLevels[i] = slevel_copy[rand_i];
	}

	free(no_repeats);
	free(slevel_copy);
}

int LoadSelectedLevel(int level)
{
	if(!gSelectedLevels)
	{
		printf("Levels not Selected");
		return -1;
	}
	if(!gSelectedLevels[level])
	{
		printf("Level %d not found", level);
		return -1;
	}
	printf("Level %d not found", level);
	ConvertFileToUseable(gSelectedLevels[level], &gParser, &gLevelData, &gLevelTokens);
	if(!gLevelData || !gLevelTokens)
	{
		printf("Unable to parse level %s", gSelectedLevels[level]);
	}
	gLevelObject = ParseToObject(gLevelTokens, gLevelData);
	if(!gLevelObject)
	{
		printf("Unable to parse level %s to object", gSelectedLevels[level]);
	}

	if(LoadLevel(gLevelObject, gLevelData))
	{
		perror("Unable to Load level");
		return -1;
	}

}

void Poll()
{
	return;
}

void UpdateStart();
void UpdateGuess();
void UpdatePlaying();

void Update()
{
	jsmntok_t *splash = NULL;
	switch(gGameState)
	{
	case(SPLASH):
		{
			if(!gSplash)
			{
				splash = FindKey(gGameTokens, SPLASH_SCREEN,gGameData);
				if(!splash)
				{
					printf("SplashScreen key not found in gameData \n");
				} else {
					gSplash = LoadSprite(FindValueFromKey(splash, SPLASH_SCREEN, gGameData), 0);
					if(!gSplash)
					{
						printf("Splash screen could not be loaded \n");
					}
				}
			}
			if(SDL_PollEvent(&gEventQ))
			{
				if(gEventQ.type == SDL_CONTROLLERBUTTONUP)
				{
					gLives = LIVES_DEFAULT;
					gGameState = START;
				}
			}
			break;
		}
	case(START):
		{
			break;
		}
	case(GUESS):
		{
			break;
		}
	case(PLAYING):
		{
			break;
		}
	}
	return;
}

void DrawSplash();
void DrawStart();
void DrawGuess();
void DrawPlaying();

void Draw()
{
	SDL_RenderClear(gRenderer);
	switch(gGameState)
	{
	case(SPLASH):
		{
			DrawSplash();
			break;
		}
	case(START):
		{
			DrawStart();
			break;
		}
	case(GUESS):
		{
			DrawGuess();
			break;
		}
	case(PLAYING):
		{
			DrawPlaying();
			break;
		}
	}
	SDL_RenderPresent(gRenderer);
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
	if(InitMenuSystem())
	{
		perror("Initialize Menu system went wrong");
		return -1;
	}
	if(LoadEntityData())
	{
		perror("Loading entity data went wrong");
		return -1;
	}
	if(LoadLevelData())
	{
		perror("Loading level data went wrong");
		return -1;
	}

	if(SelectLevels())
	{
		perror("Selecting levels went wrong");
		return -1;
	}

	LoadSelectedLevel(0);
	PrintObject(gLevelObject, gLevelData);
	test_sprite = LoadSprite("Sprite/UI/NESController.png",0);
	test_sprite->mCurrentFrame = LoadAnimation(test_sprite->mSize.x, test_sprite->mSize.y, test_sprite->mSize.x, test_sprite->mSize.y);
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
		SDL_Delay(17);
	}
	return 0;
}

void Shutdown()
{

	return;
}


void DrawSplash()
{
	if(gSplash)
	{
		if(DrawSprite(gSplash, &ZeroPos, gRenderer))
		{
			printf("Couldn't draw splash: %s \n", SDL_GetError());
		}
	}
	return;
}

void DrawStart()
{
	if(gSplash)
	{
		if(DrawSprite(gSplash, &ZeroPos, gRenderer))
		{
			printf("Couldn't draw splash: %s \n", SDL_GetError());
		}
	}
	return;
}

void DrawGuess()
{

	return;
}

void DrawPlaying()
{

	return;
}