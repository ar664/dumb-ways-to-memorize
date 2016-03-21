#ifndef __DUMB_PHYSICS__H
#define __DUMB_PHYSICS__H

#include "entity.h"

typedef enum
{
	PHYSICS_BASE_SPEED_X	= 5,
	PHYSICS_BASE_SPEED_Y	= 10,
	PHYSICS_MAX_SPEED		= 50,
	PHYSICS_MAX
}physics_laws_t;

typedef struct physics_s physics_t;

struct physics_s
{
	int dummy_var;
};

void RunPhysics(); //Double For Loop Time
int CheckCollision(entity_t *self, entity_t *other);
void DoCollision(entity_t *self, entity_t *other);

#endif
