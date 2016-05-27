#ifndef __PLAYER_CONTROLLER__H
#define __PLAYER_CONTROLLER__H

#ifdef __WIN32
	#include <SDL.h>
	#include <SDL_keycode.h>
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_keycode.h>
#endif
#define BUTTON_NO_INPUT	-1

extern SDL_GameController *gController;		/**< The controller */
extern SDL_GameControllerButton gButtonQ;	/**< The button qeueu updated with the current button pressed*/

enum KEYBOARD_BINDINGS
{
	LEFT_BUTTON		=SDL_SCANCODE_A,
	RIGHT_BUTTON		=SDL_SCANCODE_D,
	A_BUTTON		=SDL_SCANCODE_I,
	B_BUTTON		=SDL_SCANCODE_O,
	START_BUTTON		=SDL_SCANCODE_P,
	DOWN_BUTTON		=SDL_SCANCODE_S,
	UP_BUTTON		=SDL_SCANCODE_W,
	SAVE_BUTTON		=SDL_SCANCODE_U,
	LTRIG_BUTTON		=SDL_SCANCODE_M,
	MAX_BUTTON		=40
};

SDL_GameControllerButton GetKeyboardButton();
SDL_GameControllerButton IsKeyPressed(int key);

#endif
