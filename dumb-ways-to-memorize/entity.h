#ifndef __ENTITY__H
#define __ENTITY__H

#include "globals.h"
#include "graphics.h"

#define MAX_ENTITIES 750

typedef struct entity_s entity_t;

/**
 * The core structure of our our entity system.
 *
 * @author	Anthony Rios
 * @date	2/17/2016
 */

struct entity_s
{
	int mHazards;
	collision_type_t mCollisionType;
	entity_state_t mEntityState;
	sprite_t **mSprites;
	char *mName;
	vec2_t mAccel;
	vec2_t mVelocity;
	vec2_t mPosition;
	void (*Think)(entity_t *self);
	void (*Touch)(entity_t *self, entity_t *other, int type);
	void (*Draw)(entity_t *self);
	void (*PowerUp)(entity_t *self);
};

extern entity_t *gEntities;
extern int gLastEntity;

extern int InitEntitySystem();
extern entity_t *InitNewEntity();
extern entity_t *FindEntity(const char *name);
extern entity_t *FindFreeEntity(int* position);
extern entity_t *LookForEntityAtPos(vec2_t position);

void FreeEntity(entity_t *ent);
void ShutdownEntitySystem();

#endif