#ifndef __GAME_H
#define __GAME_H

#define	SCREEN_RES_X	1280
#define	SCREEN_RES_Y	720

#define	JSON_FILE		"GameData.json"		
#define ENTITY_FILE		"Entities.json"
#define SPLASH_SCREEN	"SplashScreen"
#define LEVELS_NOT_DEFAULT "LevelsPerGame"
#define LIVES_DEFAULT	3
#define LEVELS_DEFAULT	6

static int	mState;
int	Setup();
int Run();
void Shutdown();

#endif