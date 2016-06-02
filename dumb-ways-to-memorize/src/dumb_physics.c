#include "dumb_physics.h"
#include "entity.h"
#include "globals.h"
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
	GameState curr_state;
	CP_ARBITER_GET_SHAPES(arb, shape1, shape2);
	if(!arb || !shape1 || !shape2)
	{
		return false;
	}
	curr_state = gGameState;
	ent1 = (entity_t*)shape1->data;
	ent2 = (entity_t*)shape2->data;
	if(ent1->Touch)
	{
		ent1->Touch(ent1, ent2);
	}
	
	//Check failsafe	
	if(curr_state != gGameState)
	{
		return false;
	}
	
	if(ent2->Touch)
	{
		ent2->Touch(ent2, ent1);
	}
	
	//Failsafe
	if(curr_state != gGameState)
	{
		return false;
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

//Key = Shape
//Data = Body
void SafeRemovePhysics(cpSpace *space, void *key, void *data)
{
	cpShape *shape;
	cpBody *body;
	entity_t *ent;
	if(!key || !data)
	{
		return;
	}
	shape = (cpShape*) key;
	body = (cpBody*) data;
	ent = (entity_t*) shape->data;
	if(space && !cpBodyIsRogue(body) )
	{
		if( !cpBodyIsStatic(body) )
		{
			if(shape) cpSpaceRemoveShape(space, shape);
			if(body) cpSpaceRemoveBody(space, body);
		} else
		{
			if(shape) cpSpaceRemoveStaticShape(space, shape);
			if(body) cpSpaceRemoveBody(space, body);
		}
	}
	
	
	if(shape) cpShapeDestroy( shape );
	if(body) cpBodyDestroy( body );
}

void RemoveEntityFromPhysics(entity_t *ent)
{
	cpSpace *space;
	if(!ent)
	{
		return;
	}
	if(!ent->mPhysicsProperties)
	{
		return;
	}
	if( cpBodyIsRogue( ent->mPhysicsProperties->body )  )
	{
		space = NULL;
	} else
	{
		space = gSpace;
	}
	if(space)
	{
		if(cpSpaceIsLocked(space))
		{
			cpSpaceAddPostStepCallback(space, SafeRemovePhysics, ent->mPhysicsProperties->shape, ent->mPhysicsProperties->body);
		} else
		{
			SafeRemovePhysics(space, ent->mPhysicsProperties->shape, ent->mPhysicsProperties->body);
		}
	} else
	{
		SafeRemovePhysics(space, ent->mPhysicsProperties->shape, ent->mPhysicsProperties->body);
	}
	
	ent->mPhysicsProperties->body = NULL;
	ent->mPhysicsProperties->shape = NULL;
	if(ent->mPhysicsProperties)
	{
		free(ent->mPhysicsProperties);
		ent->mPhysicsProperties = NULL;
	}
}

void SafeUpdatePhysics(cpSpace *space, void *key, void *data)
{
	if(!space)
	{
		return;
	}

	cpSpaceReindexStatic(space);
}

void UpdatePhysics()
{
	if(!gSpace)
	{
		return;
	}
	if( cpSpaceIsLocked(gSpace) )
	{
		cpSpaceAddPostStepCallback( gSpace, SafeUpdatePhysics, NULL, NULL );
	} else
	{
		SafeUpdatePhysics(gSpace, NULL, NULL);
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
	if(!gSpace)
	{
		return;
	}
	cpSpaceStep((cpSpace*) gSpace, 1.0f/60.0f);

}

entity_t * AddPhyicsToEntity(entity_t* ent)
{
	if(!ent)
	{
		return NULL;
	}
	ent->mPhysicsProperties = (physics_t*) malloc(sizeof(physics_t));
	if(!ent->mPhysicsProperties || !ent->mSprites)
	{
		printf("Alloc physics for ent error \n");
		if(ent->mPhysicsProperties)
		{
			free(ent->mPhysicsProperties);
		}
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

