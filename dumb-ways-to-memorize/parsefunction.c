#include "globals.h"
#include "parsefunction.h"
#include "SDL.h"
#include <string.h>
#include <stdio.h>
#include "parseentity.h"
#include "dumb_physics.h"
#include <chipmunk\chipmunk.h>

//Function Globals
char *FunctionNames[] = {"move", "destroy", "spawn", "edit", "nullify", 0};
void (*FunctionSymbols[]) = {Move, Destroy, Spawn, Edit, Nullify, 0};

void (*ParseToFunction(const char *name))
{
	int i;

	for(i = 0; FunctionNames[i]; i++)
	{
		if(!strcmp(FunctionNames[i], name))
		{
			return FunctionSymbols[i];
		}
	}
	return NULL;
}

void Move(entity_t *targ, entity_t **info, void *extra)
{
	cpVect cpPos;
	if(!targ || !extra)
	{
		printf("Failed to do move , invalid target/info \n");
		return;
	}
	if(!targ->mPhysicsProperties)
	{
		printf("Failed to do move , no target/info physics properties \n");
		return;
	}
	cpPos = Vec2Cp((vec2_t*)extra);
	if(extra)
	{
		cpBodySetPos(targ->mPhysicsProperties->body, cpPos);
	}
	

}

void Destroy(entity_t *targ, entity_t **info, void *extra)
{
	if(!targ)
	{
		return;
	}
	FreeEntity(targ);
}

void Spawn(entity_t *targ, entity_t **info, void *extra)
{
	entity_t *spawned;
	cpVect cpPos, cpSpeed;
	if(!targ || !info)
	{
		printf("Spawn given blank targ/info \n");
		return;
	}
	if(!targ->mPhysicsProperties || !(*info)->mPhysicsProperties)
	{
		printf("Spawn given info without physics \n");
		return;
	}

	spawned = InitNewEntity();
	if (!spawned)
	{
		printf("Max entities reached can't spawn any more \n");
		return;
	}

	memcpy(spawned, *info, sizeof(entity_t));
	AddPhyicsToEntity(spawned);
	if(!spawned->mPhysicsProperties)
	{
		printf("Unable to spawn entity, physics could not be added. \n");
		return;
	}
	spawned->mPhysicsProperties->body->p = (*info)->mPhysicsProperties->body->p;
	cpPos = cpvadd(cpBodyGetPos(spawned->mPhysicsProperties->body), cpBodyGetPos(targ->mPhysicsProperties->body));

	if(targ->mDirection == DIR_RIGHT)
	{
		cpPos.x += targ->mSprites[ANIMATION_IDLE]->mSize.x + 10;
		cpBodySetPos(spawned->mPhysicsProperties->body, cpPos);
		cpSpeed.y = 0;
		cpSpeed.x = PHYSICS_BASE_SPEED_X;
		cpBodyApplyForce( spawned->mPhysicsProperties->body, cpSpeed, cpvzero);
	} else
	{
		cpPos.x -= targ->mSprites[ANIMATION_IDLE]->mSize.x + 10;
		cpBodySetPos(spawned->mPhysicsProperties->body, cpPos);
		cpSpeed.y = 0;
		cpSpeed.x = -PHYSICS_BASE_SPEED_X;
		cpBodyApplyForce( spawned->mPhysicsProperties->body, cpSpeed, cpvzero);
	}
	AddEntityToPhysics(spawned);
}

void Edit(entity_t *targ, entity_t **info, void *extra)
{
	entity_member_t *member;
	if(!targ || !extra)
	{
		printf("Null edit, not doing \n");
		return;
	}
	member = (entity_member_t*) extra;
	ApplyEntityMember(targ, member);
	
}

void Nullify(entity_t *targ, entity_t **info, void *extra)
{
	if(!targ)
	{
		printf("Null given null target \n");
	}
	targ->Think = NULL;
}
