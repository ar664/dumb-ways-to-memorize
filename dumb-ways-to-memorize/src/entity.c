#include "globals.h"
#include "entity.h"
#include "graphics.h"
#include "controller.h"
#include <stdio.h>
#include <math.h>
#include "player.h"
#include "parselevel.h"
#include "dumb_physics.h"

entity_t *gEntities = NULL;
entity_t *gEditorEntity = NULL;
int gLastEntity = 0;
int gEditorCount = 0;
char **Hazards_str = NULL;
char *Collisions_str[] = {"static", "independent", "ragdoll", "clip", 0};
char *EntityStates_str[] = {"alive", "dead", "other", 0};

vec2_t EntityPosition(entity_t *ent)
{
	cpVect position;
	vec2_t vec_pos;
	position = cpBodyGetPos(ent->mPhysicsProperties->body);
	vec_pos.x = position.x;
	vec_pos.y = position.y;

	return vec_pos;
}

vec2_t EntityDrawPosition(entity_t *ent)
{
	cpVect position;
	vec2_t vec_pos;
	position = cpBodyGetPos(ent->mPhysicsProperties->body);
	vec_pos.x = position.x - ent->mSprites[0]->mSize.x/2;
	vec_pos.y = position.y - ent->mSprites[0]->mSize.y/2;

	return vec_pos;
}

void DrawGeneric(entity_t *self)
{
	vec2_t pos_vec;
#ifdef __COLLISION_DBG
#define __COLLISION_DBG
	SDL_Rect collision_dst;
	cpBB collision_bb;
#endif
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
		pos_vec = EntityDrawPosition(self);
		DrawSprite(self->mAnimation, self->mCurrentFrame, &pos_vec, gRenderer, self->mDirection);
		if( SDL_GetTicks() > self->mNextFrameChange )
		{
			self->mCurrentFrame = self->mCurrentFrame >= self->mAnimation->mFrames ? 0 : self->mCurrentFrame+1;
			self->mNextFrameChange = SDL_GetTicks() + self->mSprites[ANIMATION_IDLE]->mMillisecondsPerFrame;
		}
	} else
	{
		pos_vec = EntityDrawPosition(self);
		DrawSprite(self->mSprites[ANIMATION_IDLE], self->mCurrentFrame, &pos_vec, gRenderer, self->mDirection);
		if( SDL_GetTicks() > self->mNextFrameChange )
		{
			self->mCurrentFrame = self->mCurrentFrame >= self->mSprites[ANIMATION_IDLE]->mFrames ? 0 : self->mCurrentFrame+1;
			self->mNextFrameChange = SDL_GetTicks() + self->mSprites[ANIMATION_IDLE]->mMillisecondsPerFrame;
		}
	}

	
#ifdef __COLLISION_DBG
#define __COLLISION_DBG
		collision_bb = cpShapeGetBB( self->mPhysicsProperties->shape);
		SDL_SetRect(&collision_dst, collision_bb.l, collision_bb.b, collision_bb.r - collision_bb.l, collision_bb.t - collision_bb.b  );
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0, 0, 0xDD);
		SDL_RenderDrawRect(gRenderer, &collision_dst);
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
#endif
}

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

	self->mNextThink = gCurrentTime + 2*UPDATE_FRAME_DELAY;
}

void ThinkCursor(entity_t *self)
{
	cpVect pos;
	int x, y;
	if(!self)
	{
		return;
	}
	if(!self->mPhysicsProperties)
	{
		return;
	}
	if(!gController)
	{
		SDL_GetMouseState(&x, &y);
		pos.x = x; pos.y = y;
	} else
	{
		pos = cpBodyGetPos(self->mPhysicsProperties->body);
		pos.x += SDL_GameControllerGetAxis(gController, SDL_CONTROLLER_AXIS_LEFTX);
		pos.y += SDL_GameControllerGetAxis(gController, SDL_CONTROLLER_AXIS_LEFTY);

	}
	if(!cpSpaceIsLocked(gSpace))
	{
		cpBodySetPos(self->mPhysicsProperties->body, pos);
	}
	self->mNextThink = gCurrentTime + UPDATE_FRAME_DELAY;
}

void ThinkPlayer(entity_t *self)
{
	//Do input control
	if(!self) return;
	if(self != gPlayer)
	{
		printf("Non player entity thinking as player \n");
	}
	if(gButtonQ != BUTTON_NO_INPUT)
	{
		DoPlayerThink(self, gButtonQ);
	} else if(SDL_GameControllerGetButton(gController, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || IsKeyPressed(LEFT_BUTTON) == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
	{
		DoPlayerThink(self, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	} else if(SDL_GameControllerGetButton(gController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || IsKeyPressed(RIGHT_BUTTON) == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
	{
		DoPlayerThink(self, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	} else
	{
		if(abs(cpBodyGetVel(self->mPhysicsProperties->body).y) < 0.5 && abs(cpBodyGetVel(self->mPhysicsProperties->body).x) < 0.5)
		{
			EntitySetAnimation(self, ANIMATION_IDLE);
		} else
		{
			EntitySetAnimation(self, ANIMATION_JUMP);
		}
		
	}
	self->mNextThink = gCurrentTime + 1; //Player always thinks
	if(self->mHealth < 0)
	{
		if(gPlayerLives < 0)
		{
			ResetGame();
		} else
		{
			ResetRun();
		}
	}
}

void ThinkEnemy(entity_t *self)
{
	if(!self || gGameState != PLAYING) 
	{
		return;
	}
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
		//Later program player search
		//if(self->mData->mAction == AI_ACTION_MOVE || self->mData->mAction == AI_ACTION_WALK)
		
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

void TouchGeneric(entity_t *self, entity_t *other)
{
	if(!other->mName)
	{
		return;
	}
	switch(other->mCollisionType)
	{
	case(COLLISION_TYPE_STATIC):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				self->mNextThink += HAZARD_STUN_FRAMES*UPDATE_FRAME_DELAY;
				EntitySetAnimation(self, ANIMATION_HIT );
			}
			break;
		}
	case(COLLISION_TYPE_RAGDOLL):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				other->mHealth -= HAZARD_DAMAGE;
				EntitySetAnimation(self, ANIMATION_HIT );
				EntitySetAnimation(other, ANIMATION_HIT );
			}
		}
	default:
		break;
	}
}

//Touch Functions
void TouchPlayer(entity_t *self, entity_t *other)
{
	if(!other->mName)
	{
		return;
	}
	switch(other->mCollisionType)
	{
	case(COLLISION_TYPE_STATIC):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				EntitySetAnimation(self, ANIMATION_HIT );
				self->mNextThink += HAZARD_STUN_FRAMES*UPDATE_FRAME_DELAY;
			}
			break;
		}
	case(COLLISION_TYPE_RAGDOLL):
		{
			if(! (other->mHazards & self->mHazards) )
			{
				self->mHealth -= HAZARD_DAMAGE;
				EntitySetAnimation(self, ANIMATION_HIT );
				self->mNextThink += HAZARD_STUN_FRAMES*UPDATE_FRAME_DELAY;
			}
			break;
		}
	default:
		break;
	}
}

void TouchEnemy(entity_t *self, entity_t *other)
{
	if(!other->mName)
	{
		return;
	}
	switch(other->mCollisionType)
	{
	case(COLLISION_TYPE_STATIC):
		{
			break;
		}
	case(COLLISION_TYPE_RAGDOLL):
		{
			break;
		}
	default:
		break;
	}
}

void TouchGoal(entity_t* self, entity_t* other)
{
	if(gGameState != PLAYING)
	{
		return;
	}	
	if(other == gPlayer)
	{
		gGameState = CountMem(gUsedPowerUps, sizeof(char*)) >= CountMem(gSelectedPowerUps, sizeof(char*)) ? START : CHOOSE;
		if(gGameState == START)
		{
			ResetGame();
		} else
		{
			GameNextLevel();
		}
		
	}
}

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
	atexit(ShutdownEntitySystem);
	return 0;
}

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

void RunEntities()
{
	int i;
	GameState currState;
	if(!gEntities)
	{
		return;
	}
	currState = gGameState;
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
		
		//Check if a game state change has occured
		if(currState != gGameState)
		{
			break;
		}
	}
}

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

entity_t* FindFreeEntity(int* position)
{
	int i;
	for(i = gLastEntity; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
		{
			if(position)
				*position = i;
			gLastEntity = i;
			return &gEntities[i];
		}
	}
	for(i = 0; i < gLastEntity; i++)
	{
		if(!gEntities[i].mName)
		{
			if(position)
				*position = i;
			gLastEntity = i;
			return &gEntities[i];
		}
	}
	return NULL;
}

entity_t *LookForEntityAtPos(vec2_t position)
{
	int i;
	cpShape *firstShape;
	cpVect pos;
	pos.x = position.x;
	pos.y = position.y;
	firstShape = cpSpacePointQueryFirst(gSpace, pos, 0xFFFFFFFF, NULL);
	if(!firstShape)
	{
		return NULL;
	}
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mPhysicsProperties)
			continue;
		if( (gEntities[i].mPhysicsProperties->shape == firstShape) )
		{
			return &gEntities[i];
		}
	}
	return NULL;
}

int Distance2Entity(entity_t* self, entity_t* other)
{
	int x, y;
	x = EntityPosition(self).x - EntityPosition(other).x;
	y = EntityPosition(self).y - EntityPosition(self).y;
	return powf(powf(x, 2) + powf(y, 2), (float) 1/2);
}

void FreeEntity(entity_t *ent)
{
	int i, numSprites;
	if(!ent)
		return;
	if(ent->mSprites)
	{
		if(gGameState == END)
		{
			numSprites = CountMem(ent->mSprites, sizeof(char*));
			for(i = 0; i < numSprites; i++)
			{
				FreeSprite(ent->mSprites[i]);
			}	
		}
	}
	if(ent->mPhysicsProperties)
	{
		RemoveEntityFromPhysics(ent);
	}
	
	memset(ent, 0, sizeof(entity_t));
			
}

void FreeNonPlayerEntities()
{
	int i;
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

void FreeEnemyEntities()
{
	int i;
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
		if(gEntities[i].Think == ThinkEnemy)
		{
			FreeEntity(&gEntities[i]);
		}
	}
}

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

entity_t *NexCachedEntity()
{
	gEditorCount++;
	if(!gEntityDictionary[gEditorCount].mName)
	{
		gEditorCount = 0;
		return &gEntityDictionary[gEditorCount];
	} else
	{
		return &gEntityDictionary[gEditorCount];
	}
}

void EntitySetAnimation(entity_t* ent, int animation)
{
	int anim_count;
	if(!ent)
	{
		return;
	}
	if( !ent->mSprites || animation > MAX_ANIMATIONS)
	{
		return;
	}
	if( !ent->mSprites[0] )
	{
		return;
	}
	anim_count = CountMem(ent->mSprites, sizeof(sprite_t**));
	//Check if we have the sprite for that animation
	if( animation >= anim_count )
	{
		return;
	}
	//Check if is the current animation already
	if(ent->mSprites[animation] == ent->mAnimation )
	{
		return;
	}

	ent->mAnimation = ent->mSprites[animation];
	ent->mCurrentFrame = 0;
}
