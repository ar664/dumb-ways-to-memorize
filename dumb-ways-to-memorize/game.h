#ifndef __GAME_H
#define __GAME_H

#define	SCREEN_RES_X	1280
#define	SCREEN_RES_Y	720

#define	JSON_FILE		"GameData.json"		
#define ENTITY_FILE		"Entities.json"


 typedef enum
{
	SPLASH =	0x1,
	START =		0x2,
	GUESS =		0x4,
	PLAYING =	0x8
}GameState;

int	mState;
int	Setup();
int Run();
void Shutdown();

extern char **gLevels;
extern char **gSelectedLevels;

#endif