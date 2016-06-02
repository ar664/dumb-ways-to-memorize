#include "globals.h"
#include "controller.h"
#include "game.h"
#include "entity.h"
#include "mystrings.h"
#include "parseobject.h"
#include "parseentity.h"
#include "parselevel.h"
#include "dumb_physics.h"
#include "dumb_ui.h"
#include "player.h"
#include "graphics.h"		   
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <chipmunk/chipmunk.h>

//All char ** should be size+1, and ending member = NULL

int exitRequest = 0;
int gLevelsPerGame = LEVELS_DEFAULT;
jsmn_parser gParser;
char **gLevels = NULL;
char **gSelectedLevels = NULL;
char **gSelectedPowerUps = NULL;
char **gUsedPowerUps = NULL;
jsmntok_t *gGameTokens;
jsmntok_t *gEntityTokens;
jsmntok_t *gLevelTokens;
object_t *gGameObject;
object_t *gEntityObject;
object_t *gLevelObject;
char *gGameData;
char *gEntityData;
char *gLevelData;
char *gEntitiesFile = NULL;
char *gPowerUpsFile = NULL;
entity_t *gEntityDictionary;
power_t *gPowerUps;					
char *gCurrentPowerUpName = NULL;   
GameState gGameState = SPLASH;		
sprite_t *gSplash = NULL;				/**< The splash screen sprite*/
vec2_t gZeroVect = {0,0};
SDL_Event gEventQ;					/**< The event qeueu update with all SDL_Events */
SDL_GameController *gController = NULL; 
SDL_GameControllerButton gButtonQ;
unsigned int gCurrentTime = 0;
float gDeltaTime = 0.0;

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
	vec2_t *screen;
	object_t *temp_obj;
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
	
	//Set Screen Dims
	temp_obj = FindObject(gGameObject, SCREEN_STRING);
	if(temp_obj)
	{
		screen = ParseToVec2(temp_obj, gGameData);
		gScreenWidth = screen->x;
		gScreenHeight = screen->y;
		printf("New Screen resolution : %d %d \n", gScreenWidth, gScreenHeight);
		free(screen);
	}
	PrintObject(gGameObject, gGameData);
	gEntitiesFile = JsmnToString(FindObject(gGameObject, ENTITIES_FILE_STR)->values, gGameData );
	gPowerUpsFile = JsmnToString(FindObject(gGameObject, POWER_UPS_STR)->values, gGameData);

	return 0;
}

int LoadEntityData()
{
	int i, objects;
	entity_t *temp_ent, *pos_ent;

	//Init Entity Parse
	if( ConvertFileToUseable(gEntitiesFile, &gParser, &gEntityData, &gEntityTokens) == -1)
	{
		printf("Failure to Parse Entity Data \n");
		return -1;
	}
	gEntityObject = ParseToObject(gEntityTokens, gEntityData);
	printf("Size of Entity tokens: %d \n", CountMem(gEntityTokens, sizeof(jsmntok_t)));
	PrintObject(gEntityObject, gEntityData);
	Hazards_str = ParseToStringArray(FindObject(gGameObject, "Hazards"), gGameData);
	
	//Set Entity Dictionary
	objects = CountMem(gEntityObject->children, sizeof(object_t));

	for(i = 0; i < objects; i++)
	{
		//Set  sprites for each object
		if(FindKey(gEntityObject->children[i].keys, "sprite(s)", gEntityData))
		{
			temp_ent = ParseToEntity(&gEntityObject->children[i], gEntityData);
			if(!temp_ent) continue;

			pos_ent = FindNextFreeCachePos();
			if(!pos_ent)
			{
				printf("Out of Cache Positions for ents");
				break;
			}
			memcpy(pos_ent, temp_ent, sizeof(entity_t));

			//if(temp) free(temp);
		} else if ( FindObject(&gEntityObject->children[i], "sprite(s)") )
		{
			temp_ent = ParseToEntity(&gEntityObject->children[i], gEntityData);
			if(!temp_ent) continue;

			pos_ent = FindNextFreeCachePos();
			if(!pos_ent)
			{
				printf("Out of Cache Positions for ents");
				break;
			}
			memcpy(pos_ent, temp_ent, sizeof(entity_t));
			//if(temp) free(temp);
		}
	}
	return 0;
}

/**
 * Loads level file names, and how many levels per game.
 *
 * @return	The level data.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
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

/**
 * After Load GameData, Before Menu.
 * Loads the power_ups in the file given.
 *
 * @return	The power up data.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
int LoadPowerUpData()
{
	jsmntok_t *power_tok;
	object_t *power_obj, *temp_obj;
	power_t *temp_power;
	char *power_str;
	int powerCount, i;

	temp_obj = FindObject(gGameObject, "PowerUps");
	if(!temp_obj)
	{
		printf("Could not find powerups in GameData");
		return -1;
	}

	gPowerUpsFile = JsmnToString(temp_obj->values, gGameData);
	ConvertFileToUseable(gPowerUpsFile, NULL, &power_str, &power_tok);
	if(!power_str || !power_tok)
	{
		printf("Could not load power ups file : %s \n", gPowerUpsFile );
	}

	power_obj = ParseToObject(power_tok, power_str);
	powerCount = CountMem(power_obj->children, sizeof(object_t));
	gPowerUps = (power_t*) malloc(sizeof(power_t)*( powerCount+1 ));
	if(!gPowerUps)
	{
		printf("Could not allocate power ups");
		return -1;
	}

	for(i = 0; i < powerCount; i++)
	{
		temp_power = ParseToPowerUp(&power_obj->children[i], power_str);
		if(!temp_power)
		{
			printf("Power up %d could not be loaded \n", i);
			continue;
		}
		gPowerUps[i] = *temp_power;
		if(temp_power) free(temp_power);
	}
	memset(&gPowerUps[powerCount], 0, sizeof(power_t));

	if(power_str) free(power_str);
	if(power_tok) free(power_tok);
	if(power_obj) FreeObject(power_obj);

	return 0;
}

/**
 * Loads menu data, from files given, which are put in gMenus
 *
 * @return	The menu data.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
int LoadMenuData()
{
	int i, menuCount;
	char *menuData, *menuLink, *temp_str;
	jsmntok_t *menuTok;
	object_t *menus, *menuObj;
	if(!gMenus)
	{
		InitMenuSystem();
	}
	menus = FindObject(gGameObject, "Menus");
	menuCount = CountMem(menus->values, sizeof(jsmntok_t)) - 1;
	for(i = 0; i < menuCount; i++)
	{
		temp_str = JsmnToString(&menus->values[i], gGameData);
		ConvertFileToUseable(temp_str, NULL, &menuData, &menuTok);
		if(!menuData || !menuTok)
		{
			continue;
		}
		menuObj = ParseToObject(menuTok, menuData);
		if(!menuObj)
		{
			if(temp_str) free(temp_str);
			if(menuData) free(menuData);
			if(menuTok) free(menuTok);
			continue;
		}
		PrintObject(menuObj, menuData);
		menuObj->name = strdup(temp_str);
		menuLink = FindValue(menuObj, "link", menuData);
		
		LoadMenu(menuObj, menuData, StrToGameState(menuLink), START);

		//Deallocate
		if(menuLink) free(menuLink);
		if(menuData) free(menuData);
		if(menuTok) free(menuTok);
		if(menuObj) FreeObject(menuObj);
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
	int i, rand_i, *no_repeats;
	int levels = 0;

	//Count 
	while(gLevels[levels])
	{
		levels++;
	}

	//Alloc
	gSelectedLevels = (char**) malloc(sizeof(char*)*(levels+1));
	no_repeats = (int*) malloc(sizeof(int)*(levels+1));
	if(!gSelectedLevels) return -1;
	if(!no_repeats) return -1;
	
	//Select
	memset(no_repeats, -1, sizeof(int)*levels);
	for (i = 0; i < levels; i++)
	{
		rand_i = rand()%levels;
		while(!CompareMemToMemArray(&rand_i, no_repeats, sizeof(int), levels ))
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

/**
 * Loads selected level.
 *
 * @param	level	The level.
 *
 * @return	The selected level.
 *
 * @see		LoadLevel , in parselevel.c
 * @author	Anthony Rios
 * @date	3/29/2016
 */
int LoadSelectedLevel(int level)
{
	if(!gSelectedLevels)
	{
		printf("Levels not Selected \n");
		return -1;
	}
	if(!gSelectedLevels[level])
	{
		printf("Level %d not found \n", level);
		return -1;
	}
	ConvertFileToUseable(gSelectedLevels[level], &gParser, &gLevelData, &gLevelTokens);
	if(!gLevelData || !gLevelTokens)
	{
		printf("Unable to parse level %s \n", gSelectedLevels[level]);
		return -1;
	}
	gLevelObject = ParseToObject(gLevelTokens, gLevelData);
	if(!gLevelObject)
	{
		if(gLevelData) free(gLevelData);
		if(gLevelTokens) free(gLevelTokens);
		printf("Unable to parse level %s to object \n", gSelectedLevels[level]);
		return -1;
	}

	if(LoadLevel(gLevelObject, gLevelData))
	{
		if(gLevelData) free(gLevelData);
		if(gLevelTokens) free(gLevelTokens);
		if(gLevelObject) free(gLevelObject);
		perror("Unable to Load level \n");
		return -1;
	}
	return 0;
}

/**
 * Polls for all events and handles them.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void Poll()
{
	if( SDL_PollEvent(&gEventQ) )
	{
		//Check for new controller
		if(gEventQ.type == SDL_CONTROLLERDEVICEADDED || gEventQ.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			gController = SDL_GameControllerOpen(0);
		}

		//Check for Button from controller
		if(gEventQ.type == SDL_CONTROLLERBUTTONDOWN)
		{
			gButtonQ = (SDL_GameControllerButton) gEventQ.cbutton.button;
			printf("Button Pressed : %d \n", gButtonQ);
		} else 
		{
			gButtonQ = (SDL_GameControllerButton) BUTTON_NO_INPUT;
		}

		//Check for Button from keyboard, if there was no button from controller
		if(gButtonQ  == -1)
		{
			gButtonQ = GetKeyboardButton();
		}

		//Check for quit
		if( gEventQ.type == SDL_QUIT)
		{
			exitRequest = 1;
			return;
		}

	}

	return;
}


void UpdatePlaying();
void UpdateEditor();

void Update()
{
	char *splash;
	gCurrentTime = SDL_GetTicks();
	switch(gGameState)
	{
	case(SPLASH):
		{
			if(!gSplash)
			{
				splash = FindValue(gGameObject, SPLASH_SCREEN, gGameData);
				if(!splash)
				{
					printf("SplashScreen key not found in gameData \n");
				} else {
					gSplash = LoadSprite(splash, 0);
					if(!gSplash)
					{
						printf("Splash screen could not be loaded \n");
					}
				}
			}
			//If you pressed a button
			if(gButtonQ != -1)
			{
				gGameState = START;
			}
			break;
		}
	case(START):
		{
			if(!gMenus)
			{
				printf("No menus loaded");
				exitRequest = 1;
				return;
			}
			if(gButtonQ != -1)
			{
				gMenus[0].Update(&gMenus[0], gButtonQ);
			}
			
			break;
		}
	case(GUESS):
		{
			if(!gMenus)
			{
				printf("No menus loaded");
				exitRequest = 1;
				return;
			}
			if(gButtonQ != -1)
			{
				gMenus[1].Update(&gMenus[1], gButtonQ);
			}
			break;
		}
	case(CHOOSE):
		{
			if(!gMenus)
			{
				printf("No menus loaded");
				exitRequest = 1;
				return;
			}
			if(gButtonQ != -1)
			{
				FindMenuFromGameState(CHOOSE)->Update(&gMenus[2], gButtonQ);
			}
			break;
		}
	case(PLAYING):
		{
			UpdatePlaying();
			break;
		}
	case(EDITOR):
		{
			UpdateEditor();
			break;
		}
	default:
		break;
	}
	
}

void DrawSplash();
void DrawStart();
void DrawGuess();
void DrawChoose();
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
	case(CHOOSE):
		{
			DrawChoose();
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
	case(EDITOR):
		{
			if(gEditorEntity->Draw)
			{
				gEditorEntity->Draw(gEditorEntity);
			}
			DrawEntities();
			break;
		}
	default:
		break;
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
	//sprite_t *test_sprite;
	//vec2_t test_vec = {0,0};
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
	if(InitAudio())
	{
		perror("Initializing audio went wrong");
		return -1;
	}
	if(InitPhysics())
	{
		perror("Initializing physics went wrong");
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
	if(InitAISystem())
	{
		perror("Initialize AI system went wrong");
		return -1;
	}
	if(LoadEntityData())
	{
		perror("Loading entity data went wrong");
		return -1;
	}
	if(LoadMenuData())
	{
		perror("Shit Happens");
		return -1;
	}
	if(LoadLevelData())
	{
		perror("Loading level data went wrong");
		return -1;
	}
	if(LoadPowerUpData())
	{
		perror("Loading entity data went wrong");
		return -1;
	}
	if(SelectLevels())
	{
		perror("Selecting levels went wrong");
		return -1;
	}
	gController = SDL_GameControllerOpen(0);
	LoadGUIforGameState((GameState)PLAYING);

	return 0;
}

/**
 * Runs the main game loop.
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
int Run()
{
	while(!exitRequest)
	{
		
		Poll();
		//Thread
		Update();
		//Thread
		Draw();
		gDeltaTime = SDL_GetTicks() - gCurrentTime;
		SDL_Delay(gDeltaTime > UPDATE_FRAME_DELAY ? 0 : UPDATE_FRAME_DELAY - gDeltaTime);
	}
	return 0;
}

void Shutdown()
{

	return;
}

/**
 * Draws the splash screen.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DrawSplash()
{
	if(gSplash)
	{
		if(DrawSprite(gSplash, NULL, &gZeroVect, gRenderer, 0))
		{
			printf("Couldn't draw splash: %s \n", SDL_GetError());
		}
	}
}

/**
 * Draws the start screen.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DrawStart()
{
	if(!gMenus)
	{
		printf("No menus loaded");
		exitRequest = 1;
		return;
	}
	gMenus[0].Draw(&gMenus[0]);
}

/**
 * Draws the screen to player for powers to choose for this run.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DrawGuess()
{
	if(!gMenus)
	{
		printf("No menus loaded");
		exitRequest = 1;
		return;
	}
	gMenus[1].Draw(&gMenus[1]);
}

/**
 * Draws the screen for the player to choose which powerUp he wants for this level.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DrawChoose()
{
	if(!gMenus)
	{
		printf("No menus loaded");
		exitRequest = 1;
		return;
	}
	gMenus[2].Draw(&gMenus[2]);

}

/**
 * Draws the main game state, which is playing the game.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DrawPlaying()
{
	DrawLevel();
	DrawEntities();
	DrawUI();
}

/**
 * Updates the playing game state.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void UpdatePlaying()
{
	RunEntities();
	RunPhysics();
}

//Physics doesn't run in editor mode
//Have to create EditorEdity
//Initialize it with first entity
void UpdateEditor()
{
	int i, x, y;
	vec2_t temp_pos;
	entity_t *temp_ent, *found_ent;
	physics_t *temp_physics;
	string_object_t *file_obj, *temp_obj,*ent_obj, *static_obj;
	FILE *file;
	char *temp_str;
	switch(gButtonQ)
	{
	case(SDL_CONTROLLER_BUTTON_DPAD_LEFT):
		{
			if(gEditorEntity->mPhysicsProperties->body->p.x <= 0)
			{
				break;
			}
			gEditorEntity->mPhysicsProperties->body->p.x -= 32;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_RIGHT):
		{
			if(gEditorEntity->mPhysicsProperties->body->p.x >= gScreenWidth)
			{
				break;
			}
			gEditorEntity->mPhysicsProperties->body->p.x += 32;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_DOWN):
		{
			if(gEditorEntity->mPhysicsProperties->body->p.y >= gScreenHeight)
			{
				break;
			}
			gEditorEntity->mPhysicsProperties->body->p.y += 32;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_UP):
		{
			if(gEditorEntity->mPhysicsProperties->body->p.y <= 0)
			{
				break;
			}
			gEditorEntity->mPhysicsProperties->body->p.y -= 32;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_A):
		{
			temp_pos = EntityPosition(gEditorEntity);

			//Set Player SpawnPoint
			if(gEditorEntity->mName)
			{
				if(!strcmp(PLAYER_STR, gEditorEntity->mName))
				{
					if(!gCurrentLevel)
					{
						gCurrentLevel = (level_t*) malloc(sizeof(level_t));
						memset(gCurrentLevel, 0, sizeof(level_t));
					}
					gCurrentLevel->mSpawnPoint = temp_pos;
					if(gCursor)
					{
						if(gCursor->mPhysicsProperties)
						{
							gCursor->mPhysicsProperties->body->p.x = temp_pos.x;
							gCursor->mPhysicsProperties->body->p.y = temp_pos.y;
						}
					}
					break;
				}
			}
			//Check if there's an entity there already
			found_ent = LookForEntityAtPos(temp_pos);
			if(!found_ent)
			{
				temp_ent = InitNewEntity();
				memcpy(temp_ent, gEditorEntity, sizeof(entity_t));
				AddPhyicsToEntity(temp_ent);
				temp_ent->mPhysicsProperties->body->p = Vec2Cp(&temp_pos);
				AddEntityToPhysics(temp_ent);
			}
			break;
		}
	case(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER):
		{
			temp_physics = gEditorEntity->mPhysicsProperties;
			memcpy(gEditorEntity, NexCachedEntity(), sizeof(entity_t));
			gEditorEntity->mPhysicsProperties = temp_physics;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_LEFTSHOULDER):
		{
			file_obj = (string_object_t*) malloc(sizeof(string_object_t));
			ent_obj = (string_object_t*) malloc(sizeof(string_object_t));
			static_obj = (string_object_t*) malloc(sizeof(string_object_t));
			if(!file_obj || !ent_obj || !static_obj)
			{
				return;
			}
			memset(file_obj, 0, sizeof(string_object_t));
			memset(ent_obj, 0, sizeof(string_object_t));
			memset(static_obj, 0, sizeof(string_object_t));
			file = fopen("auto_level.txt", "w");
			if(!file)
			{
				return;
			}

			//Set Some Variables to Make file loadable
			AddKVPair2StrObj(file_obj, G_NAME_STR, "Auto Created Level" );
			ent_obj->name = LevelGlobalObjectNames[LEVEL_G_OBJECT_ENEMIES];
			static_obj->name = LevelGlobalObjectNames[LEVEL_G_OBJECT_OBJECTS];

			if(gCurrentLevel)
			{
				temp_str = Ints2Str(2, gCurrentLevel->mSpawnPoint.x, gCurrentLevel->mSpawnPoint.y );
				if(temp_str)
				{
					AddKVPair2StrObj(file_obj, LevelGlobalObjectNames[LEVEL_G_OBJECT_SPAWN], temp_str);
				}

			}
			for(i = 0; i < MAX_ENTITIES; i++)
			{
				if(!gEntities[i].mName || !gEntities[i].mPhysicsProperties)
				{
					continue;
				}
				temp_obj = (string_object_t*) malloc(sizeof(string_object_t));
				if(!temp_obj)
				{
					printf("Failure to record %s in level editor. Malloc error. \n", gEntities[i].mName);
					continue;
				}
				memset(temp_obj, 0, sizeof(string_object_t));
				temp_obj->name = gEntities[i].mName;
				AddKVPair2StrObj(temp_obj, LevelLocalObjectNames[LEVEL_L_OBJECT_OBJECT], gEntities[i].mName );
				x = gEntities[i].mPhysicsProperties->body->p.x;
				y = gEntities[i].mPhysicsProperties->body->p.y;
				temp_str = Ints2Str(2, x, y);
				if(!temp_str)
				{
					printf("Failure to record %s's position in level editor. \n", gEntities[i].mName);
					continue;
				}
				AddKVPair2StrObj(temp_obj, LevelLocalOptionNames[LEVEL_L_OPTION_POSITION], temp_str);
				if(gEntities[i].mHealth)
				{
					AddObject2StrObj(ent_obj, temp_obj);
				} else
				{
					AddObject2StrObj(static_obj, temp_obj);
				}
				
			}
			AddObject2StrObj(file_obj, ent_obj);
			AddObject2StrObj(file_obj, static_obj);
			WriteStringObjectToFile(file_obj, file, 0);
			fclose(file);
			printf("Saved created level to 'auto_level.txt' \n" );
			ResetGame();
			break;
		}
	default:
		break;
	}
}

void FreeLevelData()
{
	if(gLevelData)
	{
		free(gLevelData);
		gLevelData = NULL;
	}
	if(gLevelTokens)
	{
		free(gLevelTokens);
		gLevelTokens = NULL;
	}
	if(gLevelObject)
	{
		FreeObject(gLevelObject);
		gLevelObject = NULL;
	}
}

void ResetGame()
{
	if(gPlayerLives < 1)
	{
		printf("You died, Game over. Start a new game \n");
	} else
	{
		printf("Returning to main menu... \n");
	}
	
	FreeNonPlayerEntities();
	FreeEntity(gPlayer);
	gPlayerLives = PLAYER_LIVES;
	gGameState = START;

	//Reset Power Up Globals
	if(gCurrentPowerUpName)
	{
		//Gets freed in free menu
		gCurrentPowerUpName = NULL;
	}
	if(gSelectedPowerUps)
	{
		free(gSelectedPowerUps);
		gSelectedPowerUps = NULL;
	}
	if(gUsedPowerUps)
	{
		free(gUsedPowerUps);
		gUsedPowerUps = NULL;
	}

	if(gCurrentPowerUp)
	{
		gCurrentPowerUp = NULL;
	}

	//Reseting power select menu
	FreeMenu( FindMenuFromGameState(CHOOSE) );

	//Reset Level Objects
	FreeLevelData();

	UpdatePhysics();	
}

void ResetRun()
{
	if(gPlayer->mHealth < 1)
	{
		printf("You died, select your powerups again \n");
		gPlayerLives--;
	}
	FreeNonPlayerEntities();
	FreeEntity(gPlayer);

	gGameState = GUESS;
	if(gCurrentPowerUp)
	{
		gCurrentPowerUp = NULL;
	}

	if(gSelectedPowerUps)
	{
		free(gSelectedPowerUps);
		gSelectedPowerUps = NULL;
	}

	if(gUsedPowerUps)
	{
		free(gUsedPowerUps);
		gUsedPowerUps = NULL;
	}

	if(gCurrentPowerUpName)
	{
		gCurrentPowerUpName = NULL;
	}

	//Reset powers 
	FreeMenu( FindMenuFromGameState(CHOOSE) );

	//Reset Level Objects
	FreeLevelData();

	UpdatePhysics();
}

void GameNextLevel()
{
	FreeNonPlayerEntities();
	FreeEntity(gPlayer);

	if(gCurrentPowerUp)
	{
		gCurrentPowerUp = NULL;
	}
	if(gCurrentPowerUpName)
	{
		gCurrentPowerUpName = NULL;
	}

	//Reset Level Objects
	FreeLevelData();

	UpdatePhysics();
}
