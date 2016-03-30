#ifndef __DUMB_PHYSICS__H
#define __DUMB_PHYSICS__H

#include "entity.h"

/** Defines the Preset Physics values */
typedef enum
{
	PHYSICS_NULL			= 0,
	PHYSICS_BASE_FRICTION	= 1,	/**< An enum constant representing the physics base friction option */
	PHYSICS_BASE_SPEED_X	= 5,	/**< An enum constant representing the physics base speed in x direction */
	PHYSICS_BASE_SPEED_Y	= 10,   /**< An enum constant representing the physics base speed in y direction */
	PHYSICS_MAX_SPEED		= 16,   /**< An enum constant representing the physics maximum speed  */
	PHYSICS_MAX
}physics_laws_t;

//typedef struct physics_s physics_t;

//struct physics_s
//{
//	int dummy_var;
//};

void RunPhysics(); //Double For Loop Time
int CheckCollision(entity_t *self, entity_t *other);
void ApplySpeedLimit(vec2_t *a);
void ApplyBounds(entity_t * ent);
void ApplyFriction(vec2_t *a);
void DoCollision(entity_t *self, entity_t *other);

#endif
