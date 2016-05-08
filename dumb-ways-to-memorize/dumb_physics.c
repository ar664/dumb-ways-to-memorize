#include "dumb_physics.h"
#include "entity.h"
#include "globals.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>

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

static cpBool CallbackCallTouchFunctions(cpArbiter *arb, cpSpace *space, void *ptr)
{
	entity_t *ent1, *ent2;
	CP_ARBITER_GET_SHAPES(arb, shape1, shape2);
	if(!arb || !shape1 || !shape2)
	{
		return false;
	}
	
	ent1 = (entity_t*)shape1->data;
	ent2 = (entity_t*)shape2->data;
	if(ent1->Touch)
	{
		ent1->Touch(ent1, ent2);
	}
	if(ent2->Touch)
	{
		ent2->Touch(ent2, ent1);
	}
	return true;
}

void AddCollisionHandlerToEntity(entity_t *ent)
{
	cpSpaceAddCollisionHandler(gSpace, ent->mCollisionType, COLLISION_TYPE_RAGDOLL, CallbackCallTouchFunctions, NULL, NULL, NULL, NULL);
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

void RemoveEntityFromPhysics(entity_t *ent)
{
	if(!ent->mPhysicsProperties)
	{
		return;
	}
	if(!ent->mPhysicsProperties->shape->space_private || !ent->mPhysicsProperties->body->space_private)
	{
		return;
	}
	cpSpaceRemoveShape(gSpace, ent->mPhysicsProperties->shape);
	cpSpaceRemoveBody(gSpace, ent->mPhysicsProperties->body);
	cpShapeFree(ent->mPhysicsProperties->shape);
	cpBodyFree(ent->mPhysicsProperties->body);
	if(ent->mPhysicsProperties)
	{
		free(ent->mPhysicsProperties);
		ent->mPhysicsProperties = NULL;
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
		printf("Ent %s position: %d %d \n", gEntities[i].mName ,EntityPosition(&gEntities[i]).x, EntityPosition(&gEntities[i]).y);

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
	if(!ent->mPhysicsProperties || !ent->mSprites[0]->name)
	{
		printf("Alloc physics for ent error \n");
		return NULL;
	}
	
	ent->mPhysicsProperties->body = cpBodyNew(1, cpMomentForBox(1, ent->mSprites[0]->mSize.x, ent->mSprites[0]->mSize.y));
	cpBodySetAngVelLimit(ent->mPhysicsProperties->body, 0.0);	
	ent->mPhysicsProperties->shape = cpBoxShapeNew(ent->mPhysicsProperties->body, ent->mSprites[0]->mSize.x, ent->mSprites[0]->mSize.y );
	ent->mPhysicsProperties->shape->bb = cpBBNew(0, ent->mSprites[0]->mSize.y, ent->mSprites[0]->mSize.x, 0);

	cpShapeSetElasticity(ent->mPhysicsProperties->shape, 0);
	cpShapeSetFriction(ent->mPhysicsProperties->shape, 0.5);
	//Give shape reference to entity it belongs to
	cpShapeSetUserData(ent->mPhysicsProperties->shape, ent);
	cpShapeSetCollisionType(ent->mPhysicsProperties->shape, ent->mCollisionType);
	return ent;
}

