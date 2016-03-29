#include "globals.h"
#include "quick_controller.h"
#include "entity.h"
#include "parsepowerup.h"

/**
 * Executes the player think operation for player input.
 *
 * @param [in,out]	player	If non-null, the player.
 * @param	button		  	The button.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void DoPlayerThink(void *player, SDL_GameControllerButton button)
{
	entity_t *ent = (entity_t*) player;
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_A):
		{
			if(ent->mVelocity.y == 0)
			{
				ent->mVelocity.y += -5;
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
			ent->mVelocity.x -= 5;
			ent->mAnimation = ANIMATION_WALK >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_WALK];
			ent->mDirection = ENTITY_DIR_LEFT;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_RIGHT):
		{
			ent->mVelocity.x += 5;
			ent->mAnimation = ANIMATION_WALK >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_WALK];;
			ent->mDirection = ENTITY_DIR_RIGHT;
			break;
		}
	default:
		return;
	}

}
