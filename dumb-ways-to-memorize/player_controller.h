#ifndef __PLAYER_CONTROLLER__H
#define __PLAYER_CONTROLLER__H
#include <SDL.h>
#include <SDL_keycode.h>

#define BUTTON_NO_INPUT	-1

extern SDL_GameController *gController;		/**< The controller */
extern SDL_GameControllerButton gButtonQ;	/**< The button qeueu updated with the current button pressed*/

enum KEYBOARD_BINDINGS
{
	LEFT_BUTTON		=SDL_SCANCODE_A,
	RIGHT_BUTTON	=SDL_SCANCODE_D,
	A_BUTTON		=SDL_SCANCODE_I,
	B_BUTTON		=SDL_SCANCODE_O,
	START_BUTTON	=SDL_SCANCODE_P,
	DOWN_BUTTON		=SDL_SCANCODE_S,
	UP_BUTTON		=SDL_SCANCODE_W,
	SAVE_BUTTON		=SDL_SCANCODE_0,
	MAX_BUTTON		=40
};

/**
 * Executes the player think operation for player input.
 *
 * @param [in,out]	player	If non-null, the player.
 * @param	button		  	The button.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DoPlayerThink(void *player, SDL_GameControllerButton button);

SDL_GameControllerButton GetKeyboardButton();

#endif