#include "globals.h"
#include "player_controller.h"
#include "entity.h"
#include "parsepowerup.h"
#include "dumb_physics.h"
#include "parselevel.h"
#include "player.h"
#include <iostream>

Uint8 *last_key_pressed;

void DoPlayerThink(void *player, SDL_GameControllerButton button)
{
	entity_t *ent = (entity_t*) player;
	cpVect Walk, Jump;
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_A):
		{
			Jump.x = 0;
			Jump.y = PLAYER_BASE_JUMP;
			
			if(cpBodyGetVel(ent->mPhysicsProperties->body).y < 2.0)
			{
				cpBodyApplyImpulse(ent->mPhysicsProperties->body, Jump, cpvzero);
				ent->mAnimation = ANIMATION_JUMP >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_JUMP];
			}
			
			break;
		}
	case(SDL_CONTROLLER_BUTTON_B):
		{
			ent->PowerUp(gCurrentPowerUp);
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_LEFT):
		{
			Walk.x = -PLAYER_BASE_SPEED;
			Walk.y = cpBodyGetVel(ent->mPhysicsProperties->body).y;
			cpBodySetVel(ent->mPhysicsProperties->body, Walk);
			ent->mAnimation = ANIMATION_WALK >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_WALK];
			ent->mDirection = ENTITY_DIR_LEFT;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_RIGHT):
		{
			Walk.x = PLAYER_BASE_SPEED;
			Walk.y = cpBodyGetVel(ent->mPhysicsProperties->body).y;
			cpBodySetVel(ent->mPhysicsProperties->body, Walk);
			ent->mAnimation = ANIMATION_WALK >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_WALK];;
			ent->mDirection = ENTITY_DIR_RIGHT;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_START):
		{
			ResetGame();
		}
	case(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER):
		{
			SaveGameState();
			ResetGame();
		}
	default:
		return;
	}

}

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
	KEYBOARD_BINDINGS binds[] = { LEFT_BUTTON, RIGHT_BUTTON, A_BUTTON, B_BUTTON, START_BUTTON, DOWN_BUTTON, UP_BUTTON, SAVE_BUTTON};
	keys = (Uint8*) SDL_GetKeyboardState(NULL);
	j = 0;
	for( i = binds[j]; j < 7; i =binds[j])
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