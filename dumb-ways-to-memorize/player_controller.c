#include "globals.h"
#include "player_controller.h"
#include "entity.h"
#include "parsepowerup.h"
#include "dumb_physics.h"
#include "parselevel.h"
#include "player.h"

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
