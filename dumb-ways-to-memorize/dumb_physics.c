#include "dumb_physics.h"
#include "entity.h"
#include "globals.h"
#include "player.h"
#include <chipmunk/chipmunk.h>
#include <stdio.h>

vec2_t gGravity = {0, 2};
cpSpace *gSpace = NULL;
cpArbiter *gArbiter = NULL;
cpBB *gBoundingBox = NULL;

int InitPhysics()
{
	gSpace = cpSpaceNew();
	cpVect gravity;
	if(!gSpace)
	{
		printf("Initializing Space From Chipmunk went wrong");
		return -1;
	}
	gravity.x = 0;
	gravity.y = 9;
	cpSpaceSetGravity( gSpace, gravity);
	cpSpaceSetIdleSpeedThreshold( gSpace, 20.0);

	return 0;
}

cpCollisionBeginFunc ExampleCallback()
{
	cpSpaceRemoveBody(gSpace, NULL);
	cpSpaceRemoveShape(gSpace, NULL);
  
	cpShapeFree(NULL);
	cpBodyFree(NULL);
	return NULL;
}

void AddCallBackToEntity(entity_t *ent, void *callback)
{
	cpSpaceAddCollisionHandler(gSpace, ent->mHazards, ent->mHazards, ExampleCallback, NULL, NULL, NULL, callback);
	cpSpaceAddPostStepCallback(gSpace, cpPostStepFunc(ExampleCallback), ent->mPhysicsProperties->shape, NULL);
}

void AddNewBodyShape(entity_t* ent)
{
	cpBody *newBody;
	cpShape *newShape;
	if(!ent)
	{
		return;
	}
	if(!ent->mSprites)
	{
		return;
	}
	if(!ent->mPhysicsProperties)
	{
		ent->mPhysicsProperties = (physics_t*) malloc(sizeof(physics_t));
		if(!ent->mPhysicsProperties)
		{
			return;
		}
	}

	newBody = cpBodyNew(1, 0);
	if(!newBody)
	{
		printf("Unable to give new body to entity \n");
		return;
	}
	ent->mPhysicsProperties->body = newBody;

	newShape = cpBoxShapeNew(ent->mPhysicsProperties->body, ent->mSprites[0]->mSize.x, ent->mSprites[0]->mSize.y);
	if(!newShape)
	{
		printf("Unable to give new shape to entity \n");
	}
	ent->mPhysicsProperties->shape = newShape;
}

void AddVelocityToEntity(entity_t *ent, float speed, cpVect direction)
{
	cpVect temp_vec;
	if(!ent)
	{
		return;
	}
	if(!ent->mPhysicsProperties)
	{
		return;
	}
	temp_vec = cpvmult(direction, speed);
	cpBodySetVel(ent->mPhysicsProperties->body, temp_vec);

}

void AddForceToEntity(entity_t *ent, float speed, cpVect direction)
{
	cpVect temp_vec;
	if(!ent)
	{
		return;
	}
	if(!ent->mPhysicsProperties)
	{
		return;
	}
	temp_vec = cpvmult(direction, speed);
	cpBodyApplyForce(ent->mPhysicsProperties->body, temp_vec, cpvzero );

}

void AddImpulseToEntity(entity_t *ent, float speed, cpVect direction)
{
	cpVect temp_vec;
	if(!ent)
	{
		return;
	}
	if(!ent->mPhysicsProperties)
	{
		return;
	}
	temp_vec = cpvmult(direction, speed);
	cpBodyApplyImpulse(ent->mPhysicsProperties->body, temp_vec, cpvzero);
}

void AddEntityToPhysics(entity_t* ent)
{
	if(!ent)
	{
		return;
	}
	if(!ent->mPhysicsProperties)
	{
		return;
	}

	cpSpaceAddBody(gSpace, ent->mPhysicsProperties->body);
	cpSpaceAddShape(gSpace, ent->mPhysicsProperties->shape);
}

void RunPhysics()
{
	cpSpaceStep((cpSpace*) gSpace, gDeltaTime);
	int i, j;

	//Collision Check
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
		{
			continue;
		}
		for(j = i; j < MAX_ENTITIES; j++)
		{
			if(i == j || !gEntities[j].mName)
			{
				continue;
			}
			if(CheckCollision(&gEntities[i], &gEntities[j]))
			{
				DoCollision(&gEntities[i], &gEntities[j]);
			}
		}
	}
}

int CheckCollision(entity_t *self, entity_t *other)
{
	return 0;
}

void DoCollision(entity_t *self, entity_t *other)
{

}

void ApplySpeedLimit(vec2_t* a)
{
	a->x = abs(a->x) > PHYSICS_MAX_SPEED ? (a->x < 0 ? -PHYSICS_MAX_SPEED : PHYSICS_MAX_SPEED) : a->x;
	a->y = abs(a->y) > PHYSICS_MAX_SPEED ? (a->y < 0 ? -PHYSICS_MAX_SPEED : PHYSICS_MAX_SPEED) : a->y;
}

void ApplyBounds(entity_t* ent)
{

}

void ApplyFriction(vec2_t* a)
{
	if(abs(a->x))
	{
		a->x += a->x < 0 ? PHYSICS_BASE_FRICTION : -PHYSICS_BASE_FRICTION;
	}
	if(abs(a->y))
	{
		a->y += a->y < 0 ? PHYSICS_BASE_FRICTION : -PHYSICS_BASE_FRICTION;
	}
}
