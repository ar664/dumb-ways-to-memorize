#include "globals.h"
#include "controller.h"
#include "entity.h"
#include "dumb_physics.h"
#include "parselevel.h"
#include "player.h"

Uint8 *last_key_pressed;

SDL_GameControllerButton KeyToButton(SDL_Scancode key)
{
	switch(key)
	{
	case(A_BUTTON): return SDL_CONTROLLER_BUTTON_A;
	case(B_BUTTON): return SDL_CONTROLLER_BUTTON_B;
	case(DOWN_BUTTON): return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	case(UP_BUTTON): return SDL_CONTROLLER_BUTTON_DPAD_UP; 
	case(LEFT_BUTTON): return SDL_CONTROLLER_BUTTON_DPAD_LEFT; 
	case(RIGHT_BUTTON): return SDL_CONTROLLER_BUTTON_DPAD_RIGHT; 
	case(START_BUTTON): return SDL_CONTROLLER_BUTTON_START; 
	case(SAVE_BUTTON): return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER; 
	case(LTRIG_BUTTON): return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	default:
		break;
	}
	return (SDL_GameControllerButton) -1;
}

SDL_GameControllerButton GetKeyboardButton()
{
	const Uint8 *keys;
	int j;
	KEYBOARD_BINDINGS i;
	KEYBOARD_BINDINGS binds[] = { LEFT_BUTTON, RIGHT_BUTTON, A_BUTTON, B_BUTTON, START_BUTTON, DOWN_BUTTON, UP_BUTTON, SAVE_BUTTON, LTRIG_BUTTON};
	keys = (Uint8*) SDL_GetKeyboardState(NULL);
	j = 0;
	for( i = binds[j]; j < 9; i =binds[j])
	{
		if(keys[i])
		{
			if(&keys[i] == last_key_pressed)
			{
				return (SDL_GameControllerButton) -1;
			}
			last_key_pressed = (Uint8*) &keys[i];
			return KeyToButton((SDL_Scancode) i);
		}
		j++;
	}
	last_key_pressed = NULL;
	return (SDL_GameControllerButton) -1;
}


SDL_GameControllerButton IsKeyPressed(int key)
{
	const Uint8 *keys;
	keys = (Uint8*) SDL_GetKeyboardState(NULL);
	if(keys[key])
	{
		return KeyToButton( (SDL_Scancode)key);
	}
	return (SDL_GameControllerButton) -1 ;
}
