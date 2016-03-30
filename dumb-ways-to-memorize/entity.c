#include "globals.h"
#include "entity.h"
#include "graphics.h"
#include "quick_controller.h"
#include <stdio.h>
#include <math.h>
#include "player.h"
#include "parselevel.h"
#include "parsepowerup.h"

entity_t *gEntities = NULL;
int gLastEntity = 0;
char **Hazards_str = NULL;
char *Collisions_str[] = {"static", "ragdoll", "clip", 0};
char *EntityStates_str[] = {"alive", "dead", "other", 0};

/**
 * Draws the entity via self->mAnimation if set, or the first sprite which is idle.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void DrawGeneric(entity_t *self)
{
	if(!self)
	{
		return;
	}
	if(!self->mSprites)
	{
		return;
	}
	if(self->mAnimation)
	{
		//IncrementFrame(self->mAnimation);
		DrawSprite(self->mAnimation, &self->mCurrentFrame, &self->mPosition, gRenderer);
	} else
	{
		//IncrementFrame(self->mSprites[ANIMATION_IDLE]);
		DrawSprite(self->mSprites[ANIMATION_IDLE], &self->mCurrentFrame, &self->mPosition, gRenderer);
	}
	
}

//Unused for now
void DrawPlayer(entity_t *self)
{
	//DrawSprite(self->mSprites, &self->mPosition, gRenderer);
}

/**
 * Generic think function, checks health and dies when health <= 0
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void ThinkGeneric(entity_t *self)
{
	if(!self)
	{
		return;
	}
	if(self->mHealth <= 0)
	{
		self->Think = FreeEntity;
	}

	self->mNextThink = gCurrentTime + 2*FRAME_DELAY;
}

/**
 * The player think function, handles lives ,gamestate switching , and input.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void ThinkPlayer(entity_t *self)
{
	//Do input control
	if(!self) return;
	if(gButtonQ != BUTTON_NO_INPUT)
	{
		DoPlayerThink(self, gButtonQ);
	} else if(SDL_GameControllerGetButton(gController, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
	{
		DoPlayerThink(self, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	} else if(SDL_GameControllerGetButton(gController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
	{
		DoPlayerThink(self, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	} else if(SDL_GameControllerGetButton(gController, SDL_CONTROLLER_BUTTON_B))
	{
		if(gCurrentPowerUp)
		{
			//If infinite use
			if(!gCurrentPowerUp->UpdateUse)
			{
				self->PowerUp(gCurrentPowerUp);
			}
		}
	}
	self->mNextThink = gCurrentTime + 1; //Player always thinks
	if(self->mHealth < 0)
	{
		if(gPlayerLives < 0)
		{
			printf("You died, Game over. Start a new game");
			FreeEntity(gPlayer);
			gPlayerLives = PLAYER_LIVES;
			FreeNonPlayerEntities();
			gGameState = START;
		} else
		{
			printf("You died, select your powerups again \n");
			gPlayerLives--;
			gGameState = GUESS;
		}
	}
}

/**
 * The modular think function for enemy entities with an AI.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void ThinkEnemy(entity_t *self)
{
	if(!self) return;
	if(!self->mData) return;
	if(self->mData->mFlags & AI_FLAG_CHECK_OBJECT)
	{
		if(!self->mData->mObjectCheck)
		{
			self->mData = self->mData->mLink;
			return;
		}
		if( Distance2Entity(self, FindEntity(self->mData->mObjectCheck)) < self->mData->mVariables[AI_VAR_CHECK])
		{
			if(GetFunctionAI(self->mData))
			{
				GetFunctionAI(self->mData)(self);
			}
		}
	} else if (self->mData->mFlags & AI_FLAG_CHECK_PLAYER)
	{
		if(Distance2Entity(self, (entity_t*)gPlayer) < self->mData->mVariables[AI_VAR_CHECK])
		{
			if(GetFunctionAI(self->mData))
			{
				GetFunctionAI(self->mData)(self);
			}
		}
	} else
	{
		if(GetFunctionAI(self->mData))
		{
			GetFunctionAI(self->mData)(self);
		}
	}

}

/**
 * Generic touch function,
 * Receives pain from touch and inflict pain on touch depending on hazard types.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 * @param	type		 	The type.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void TouchGeneric(entity_t *self, entity_t *other, int type)
{
	switch(type)
	{
	case(COLLISION_TYPE_STATIC):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				self->mAnimation = ANIMATION_HIT >= CountMem(self->mSprites, sizeof(sprite_t*)) ? NULL : self->mSprites[ANIMATION_HIT];
				self->mNextThink += HAZARD_STUN_FRAMES*FRAME_DELAY;
			}
			break;
		}
	case(COLLISION_TYPE_RAGDOLL):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				other->mHealth -= HAZARD_DAMAGE;
				self->mAnimation = ANIMATION_HIT >= CountMem(self->mSprites, sizeof(sprite_t*)) ? NULL : self->mSprites[ANIMATION_HIT];
			}
		}
	default:
		break;
	}
}

//Touch Functions
void TouchPlayer(entity_t *self, entity_t *other, int type)
{
	switch(type)
	{
	case(COLLISION_TYPE_STATIC):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				self->mAnimation = ANIMATION_HIT >= CountMem(self->mSprites, sizeof(sprite_t*)) ? NULL : self->mSprites[ANIMATION_HIT];
				self->mNextThink += HAZARD_STUN_FRAMES*FRAME_DELAY;
			}
			break;
		}
	case(COLLISION_TYPE_RAGDOLL):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				self->mAnimation = ANIMATION_HIT >= CountMem(self->mSprites, sizeof(sprite_t*)) ? NULL : self->mSprites[ANIMATION_HIT];
				self->mNextThink += HAZARD_STUN_FRAMES*FRAME_DELAY;
			}
			break;
		}
		
	}
}

void TouchEnemy(entity_t *self, entity_t *other, int type)
{
	switch(other->mCollisionType)
	{
	case COLLISION_TYPE_STATIC:
		break;
	case COLLISION_TYPE_RAGDOLL:
		break;
	default:
		break;
	}
}

/**
 * The touch function for the goal entity / flag.
 * Switches gamestate based on if you won or should go to the next level.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other entity it touched.
 * @param	type		 	The type.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void TouchGoal(entity_t* self, entity_t* other, int type)
{
	if(other == gPlayer)
	{
		gGameState = CountMem(gUsedPowerUps, sizeof(char*)) >= CountMem(gSelectedPowerUps, sizeof(char*)) ? START : CHOOSE;
	}
}

/**
 * Init entity system.
 *
 * @return	An int of failure state. 0 if good, -1 if error.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

int InitEntitySystem()
{
	if( (gEntities = (entity_t*) malloc(sizeof(entity_t)*MAX_ENTITIES)) == NULL )
	{
		printf("Couldn't alloc EntitySys");
		return -1;
	}
	if( (gEntityDictionary = (entity_t*) malloc(sizeof(entity_t)*MAX_ENTITIES)) == NULL )
	{
		printf("Couldn't alloc EntitySys");
		return -1;
	}
	memset(gEntities, 0, sizeof(entity_t)*MAX_ENTITIES);
	memset(gEntityDictionary, 0, sizeof(entity_t)*MAX_ENTITIES);
	gLastEntity = 0;
	return 0;
}

/**
 * Init new entity.
 *
 * @return	null if it fails, else a pointer to an entity_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

entity_t *InitNewEntity()
{
	int pos;
	entity_t *retVal;
	if(gEntities == NULL)
	{
		printf("Entity system unintialiazeed");
		exit(-1);
	}

	retVal = FindFreeEntity(&pos);
	gLastEntity = pos;
	return retVal;
}

/**
 * Searches for the next free cache position in gEntitiesDictionary.
 *
 * @return	null if it fails, else the found free cache position.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

entity_t* FindNextFreeCachePos()
{
	int i;
	for(i = 0; gEntityDictionary[i].mName; i++)
	{
		;
	}
	if(&gEntityDictionary[i] > &gEntityDictionary[MAX_ENTITIES])
	{
		return NULL;
	}
	return &gEntityDictionary[i];
}

/**
 * Draw entities, if they have draw functions.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void DrawEntities()
{
	int i;
	if(!gEntities)
	{
		return;
	}
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].Draw)
		{
			continue;
		}
		gEntities[i].Draw(&gEntities[i]);
	}
}

/**
 * Executes the entities think functions.
 *
 * @note	I think I have the logic wrong..
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void RunEntities()
{
	int i;
	if(!gEntities)
	{
		return;
	}
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].Think || !gEntities[i].mName)
		{
			continue;  
		}
		if( gCurrentTime > gEntities[i].mNextThink)
		{
			gEntities[i].Think(&gEntities[i]);
		}
	}
}

/**
 * Searches for the cached entity whose name matches given str.
 *
 * @param	name	The name.
 *
 * @return	null if it fails, else the found cached entity.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

entity_t* FindCachedEntity(const char* name)
{
	int i;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntityDictionary[i].mName)
			break;
		if(!strcmp(name, gEntityDictionary[i].mName))
		{
			return &gEntityDictionary[i];
		}
	}
	return NULL;
}

/**
 * Searches for the first entity with the name of given str.
 *
 * @param	name	The name.
 *
 * @return	null if it fails, else the found entity.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

entity_t* FindEntity(const char* name)
{
	int i;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
			continue;
		if(!strcmp(name, gEntities[i].mName))
		{
			return &gEntities[i];
		}
	}
	return NULL;
}

/**
 * Searches for the first free entity.
 *
 * @param [in,out]	position	If non-null, the position.
 *
 * @return	null if it fails, else the found free entity.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

entity_t* FindFreeEntity(int* position)
{
	int i;
	for(i = gLastEntity; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
		{
			if(position)
				*position = i;
			return &gEntities[i];
		}
	}
	for(i = 0; i < gLastEntity; i++)
	{
		if(!gEntities[i].mName)
		{
			if(position)
				*position = i;
			return &gEntities[i];
		}
	}
	return NULL;
}

/**
 * Look for entity at position.
 *
 * @param	position	The position.
 *
 * @return	null if it fails, else a pointer to an entity_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

entity_t *LookForEntityAtPos(vec2_t position)
{
	int i;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
			continue;
		if( (gEntities[i].mPosition.x > position.x > gEntities[i].mPosition.x + gEntities[i].mSprites[0]->mSize.x)
			&& (gEntities[i].mPosition.y > position.y > gEntities[i].mPosition.x + gEntities[i].mSprites[0]->mSize.y))
		{
			return &gEntities[i];
		}
	}
	return NULL;
}

/**
 * Distance 2 entity other from entity self.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other entity.
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

int Distance2Entity(entity_t* self, entity_t* other)
{
	int x, y;
	x = self->mPosition.x - other->mPosition.x;
	y = self->mPosition.y - self->mPosition.y;
	return powf(powf(x, 2) + powf(y, 2), (float) 1/2);
}

/**
 * Free entity.
 * 
 * @note	right now it just acts like memset(0) on entities in the global Entities scope.
 * 			But it free's any other data that isn't globally set.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void FreeEntity(entity_t *ent)
{
	int i, isGlobal = 0;
	if(!ent)
		return;
	i = 0;
	//if(ent->mSprites)
	//{
		//while(ent->mSprites[i])
		//{
			//FreeSprite(ent->mSprites[i]);
			//i++;
		//}
		//free(ent->mSprites);
	//}
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(ent == &gEntities[i])
		{
			isGlobal = 1;
			memset(ent, 0, sizeof(entity_t));
		}
	}
	if(!isGlobal)
	{
		free(ent);
	}

}

/**
 * Frees the non player entities, use for Destroy World.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void FreeNonPlayerEntities()
{
	int i , entities;
	if(!gEntities)
	{
		return;
	}

	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(&gEntities[i] == (entity_t*) gPlayer)
		{
			continue;
		}
		FreeEntity(&gEntities[i]);
	}
}

/**
 * Shutdown entity system, frees all the entities.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

void ShutdownEntitySystem()
{
	int i;
	if(!gEntities)
		return;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
			continue;
		FreeEntity(&gEntities[i]);
	}
	free(gEntities);
}

