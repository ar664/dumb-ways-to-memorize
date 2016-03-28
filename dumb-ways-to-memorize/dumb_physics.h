#ifndef __DUMB_PHYSICS__H
#define __DUMB_PHYSICS__H

#include "entity.h"

typedef enum
{
	PHYSICS_NULL			= 0,
	PHYSICS_BASE_FRICTION	= 1,
	PHYSICS_BASE_SPEED_X	= 5,
	PHYSICS_BASE_SPEED_Y	= 10,
	PHYSICS_MAX_SPEED		= 32,
	PHYSICS_MAX
}physics_laws_t;

typedef struct physics_s physics_t;

struct physics_s
{
	int dummy_var;
};

void RunPhysics(); //Double For Loop Time
int CheckCollision(entity_t *self, entity_t *other);
void ApplySpeedLimit(vec2_t *a);
void ApplyBounds(vec2_t *a);
void ApplyFriction(vec2_t *a);
void DoCollision(entity_t *self, entity_t *other);

#endif
