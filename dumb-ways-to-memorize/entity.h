#ifndef __ENTITY__H
#define __ENTITY__H

#include "globals.h"
#include "graphics.h"

typedef enum
{
	ENTITY_STATE_DEAD,
	ENTITY_STATE_ALIVE,
	ENTITY_STATE_OTHER
}entity_state_t;

typedef enum
{
	COLLISION_TYPE_STATIC,
	COLLISION_TYPE_RAGDOLL,
	COLLISION_TYPE_CLIP
}collision_type_t;

typedef struct entity_s entity_t;

struct entity_s
{
	int mHazards;
	collision_type_t mCollisionType;
	entity_state_t mEntityState;
	sprite_t **mSprites;
	vec2_t mAccel;
	vec2_t mVelocity;
	vec2_t mPosition;
	void (*Think)(entity_t *self);
	void (*Touch)(entity_t *self, entity_t *other, int type);
	void (*Draw)(entity_t *self);
	void (*PowerUp)(entity_t *self);
};

extern entity_t *gEntities;
extern int gNumEntities;

extern int InitEntitySystem();
extern entity_t *InitNewEntity();
extern entity_t *LookForEntity(vec2_t position);

#endif