#include "globals.h"
#include "quick_controller.h"
#include "entity.h"

void DoPlayerThink(void *player, SDL_GameControllerButton button)
{
	entity_t *ent = (entity_t*) player;
	switch(button)
	{
	case(SDL_CONTROLLER_BUTTON_A):
		{
			if(ent->mPosition.y == (gScreenHeight + ent->mSprites[0]->mSize.y))
			{
				ent->mVelocity.y += 5;
				ent->mAnimation = ANIMATION_JUMP >= CountMem(ent->mSprites, sizeof(sprite_t*)) ? NULL : ent->mSprites[ANIMATION_JUMP];
			}
			
			break;
		}
	case(SDL_CONTROLLER_BUTTON_B):
		{
			ent->PowerUp(ent);
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