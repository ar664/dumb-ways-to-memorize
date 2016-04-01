#include "globals.h"
#include "player_controller.h"
#include "entity.h"
#include "parsepowerup.h"
#include "player.h"

void DoPlayerThink(void *player, SDL_GameControllerButton button)
{
	entity_t *ent = (entity_t*) player;
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_A):
		{
			if(ent->mVelocity.y == 0)
			{
				ent->mVelocity.y += PLAYER_BASE_JUMP;
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
			ent->mVelocity.x -= PLAYER_BASE_SPEED;
			ent->mAnimation = ANIMATION_WALK >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_WALK];
			ent->mDirection = ENTITY_DIR_LEFT;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_DPAD_RIGHT):
		{
			ent->mVelocity.x += PLAYER_BASE_SPEED;
			ent->mAnimation = ANIMATION_WALK >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_WALK];;
			ent->mDirection = ENTITY_DIR_RIGHT;
			break;
		}
	case(SDL_CONTROLLER_BUTTON_START):
		{
			ResetGame();
		}
	default:
		return;
	}

}
