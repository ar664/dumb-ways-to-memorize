#ifndef __DUMB_PHYSICS__H
#define __DUMB_PHYSICS__H

#include "entity.h"
#include "chipmunk/chipmunk.h"

/** Defines the Preset Physics values */
typedef enum
{
	PHYSICS_NULL			= 0,
	PHYSICS_BASE_FRICTION	= 1,	/**<  physics base friction option */
	PHYSICS_BASE_SPEED_X	= 5,	/**<  physics base speed in x direction */
	PHYSICS_BASE_SPEED_Y	= 10,   /**<  physics base speed in y direction */
	PHYSICS_MAX_SPEED		= 16,   /**<  physics maximum speed  */
	PHYSICS_MAX
}physics_laws_t;

typedef struct physics_s physics_t;

extern cpSpace *gSpace;
extern cpArbiter *gArbiter;
extern cpBB *gBoundingBox;

int InitPhysics();
void ShutdownPhysics();

void AddEntityToPhysics(entity_t *ent);
void RemoveEntityFromPhysics(entity_t *ent);

struct physics_s
{
	cpBody *body;
	cpShape *shape;
};

void AddVelocityToEntity(entity_t *ent, float speed, cpVect direction);
void AddForceToEntity(entity_t *ent, float speed, cpVect direction);
void AddCollisionHandlerToEntity(entity_t *ent);
void AddImpulseToEntity(entity_t *ent, float speed, cpVect direction);
void SetCpCollisionType(entity_t *ent);

/**
 * Executes physics.
 * Applies velocity, acceleration, gravity, and friction.
 * Limits velocity, acceleration, and sets bounds.
 * 
 * @author	Anthony Rios
 * @date	3/20/2016
 */
void RunPhysics(); 


/**
 * To be used when leaving a level. It reindex's the current shapes in the space.
 * This is to prevent chipmunk from keeping the static shapes in its cache.
 */
void UpdatePhysics();

/**
 * Check collision between entities. (AABB)
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 *
 * @return	An int 1 if colliding, 0 if not.
 *
 * @author	Anthony Rios
 * @date	3/20/2016
 */
int CheckCollision(entity_t *self, entity_t *other);

/**
 * Executes the collision operation.
 * Place self farthest x or y away from other, vice versa.
 * And negative the velocity to half of orginal collision vel, for bounce back.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 *
 * @author	Anthony Rios
 * @date	3/20/2016
 */
void DoCollision(entity_t *self, entity_t *other);

/**
 * Applies the speed limit to vector a, defined by physics max speed.
 *
 * @param [in,out]	a	If non-null, the vec2_t to process.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ApplySpeedLimit(vec2_t *a);

/**
 * Applies the bounds to specified ent, defined by the screen dimensions.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ApplyBounds(entity_t * ent);

/**
 * Applies the friction to vector a, for the nonzero values.
 * Uses Physics_base_friction for friction coefficient.
 *
 * @param [in,out]	a	If non-null, the vec2_t to process.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ApplyFriction(vec2_t *a);

/**
 * Init entity physics properties. By assigning new body and shape.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	4/18/2016
 */

entity_t * AddPhyicsToEntity(entity_t *ent);

#endif
