#ifndef __QUICK_CONTROLLER__H
#define __QUICK_CONTROLLER__H
#include <SDL.h>

#define BUTTON_NO_INPUT	-1

extern SDL_GameController *gController;
extern SDL_GameControllerButton gButtonQ;

void DoPlayerThink(void *player, SDL_GameControllerButton button);


#endif