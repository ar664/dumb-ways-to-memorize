#ifndef __PLAYER_CONTROLLER__H
#define __PLAYER_CONTROLLER__H
#include <SDL.h>

#define BUTTON_NO_INPUT	-1

extern SDL_GameController *gController;		/**< The controller */
extern SDL_GameControllerButton gButtonQ;	/**< The button qeueu updated with the current button pressed*/

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


#endif