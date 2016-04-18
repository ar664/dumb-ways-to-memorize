#include "dumb_physics.h"
#include "entity.h"
#include "globals.h"
#include "player.h"
#include <stdio.h>

vec2_t gGravity = {0, 2};
cpSpace *gSpace = NULL;
cpArbiter *gArbiter = NULL;
cpBB *gBoundingBox = NULL;

int InitPhysics()
{
	gSpace = cpSpaceNew();
	cpVect gravity;
	cpFloat damping = PHYSICS_BASE_FRICTION - 0.1, max_speed = PHYSICS_MAX_SPEED;
	if(!gSpace)
	{
		printf("Initializing Space From Chipmunk went wrong");
		return -1;
	}
	gravity.x = 0;
	gravity.y = 9;
	cpSpaceSetGravity( gSpace, gravity);
	cpSpaceSetDamping(gSpace, damping);
	cpSpaceSetIdleSpeedThreshold( gSpace, max_speed);

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
	cpSpaceAddCollisionHandler(gSpace, ent->mHazards, ent->mHazards, ExampleCallback(), NULL, NULL, NULL, callback);
	cpSpaceAddPostStepCallback(gSpace, cpPostStepFunc(ExampleCallback), ent->mPhysicsProperties->shape, NULL);
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
	switch(ent->mCollisionType)
	{
	case(COLLISION_TYPE_STATIC):
		{
			cpSpaceRemoveBody(gSpace, ent->mPhysicsProperties->body);
			cpSpaceConvertBodyToStatic(gSpace, ent->mPhysicsProperties->body);
			cpSpaceAddStaticShape(gSpace, ent->mPhysicsProperties->shape);
			break;
		}
	case(COLLISION_TYPE_INDEPENDENT):
		{
			cpSpaceAddShape(gSpace, ent->mPhysicsProperties->shape);
			break;
		}
	case(COLLISION_TYPE_RAGDOLL):
		{
			cpSpaceAddShape(gSpace, ent->mPhysicsProperties->shape);
			break;
		}
	case(COLLISION_TYPE_CLIP):
		{
			cpSpaceAddShape(gSpace, ent->mPhysicsProperties->shape);
			break;
		}
	default:
		break;
	}
	
}

void SetCpCollisionType(entity_t *ent)
{
	if(!ent)
	{
		return;
	}
	if(!ent->mPhysicsProperties)
	{
		return;
	}
	if(!ent->mPhysicsProperties->body->space_private)
	{
		printf("Entity %s not added to a space! \n", ent->mName);
		return;
	}
	switch(ent->mCollisionType)
	{
	case(COLLISION_TYPE_STATIC):
		{
			cpSpaceRemoveBody(gSpace, ent->mPhysicsProperties->body);
			cpSpaceConvertBodyToStatic(gSpace, ent->mPhysicsProperties->body);
			cpSpaceAddBody(gSpace, ent->mPhysicsProperties->body);
			break;
		}
	case(COLLISION_TYPE_INDEPENDENT):
		{
			cpSpaceRemoveShape(gSpace, ent->mPhysicsProperties->shape);
			cpSpaceRemoveBody(gSpace, ent->mPhysicsProperties->body);
			break;
		}
	case(COLLISION_TYPE_RAGDOLL):
		{
			cpSpaceRemoveBody(gSpace, ent->mPhysicsProperties->body);
			cpSpaceConvertBodyToDynamic(gSpace, ent->mPhysicsProperties->body, 1, cpMomentForBox(1, ent->mSprites[0]->mSize.x, ent->mSprites[0]->mSize.y));
			cpSpaceAddBody(gSpace, ent->mPhysicsProperties->body);
			break;
		}
	case(COLLISION_TYPE_CLIP):
		{
			cpSpaceRemoveShape(gSpace, ent->mPhysicsProperties->shape);
			cpSpaceRemoveBody(gSpace, ent->mPhysicsProperties->body);
			break;
		}
	default:
		break;
	}
}

void PrePhysics()
{
	int i;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
		{
			continue;
		}
		printf("Ent %s position: %d %d \n", gEntities[i].mName ,gEntities[i].GetPosition().x, gEntities[i].GetPosition().y);

	}
}

void RunPhysics()
{
	cpSpaceStep((cpSpace*) gSpace, 1.0f/60.0f);
	//PrePhysics();

	// Old Physics Code
	/*
	int i, j;
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
	*/
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

entity_t * AddPhyicsToEntity(entity_t* ent)
{
	if(!ent)
	{
		return NULL;
	}
	ent->mPhysicsProperties = (physics_t*) malloc(sizeof(physics_t));
	if(!ent->mPhysicsProperties)
	{
		printf("Alloc physics for ent error \n");
		return NULL;
	}

	ent->mPhysicsProperties->body = cpBodyNew(1, cpMomentForBox(1, ent->mSprites[0]->mSize.x, ent->mSprites[0]->mSize.y));
	ent->mPhysicsProperties->shape = cpBoxShapeNew(ent->mPhysicsProperties->body, ent->mSprites[0]->mSize.x, ent->mSprites[0]->mSize.y );
	ent->mPhysicsProperties->shape->bb = cpBBNew(0,gScreenHeight, gScreenWidth, 0);
	return ent;
}
