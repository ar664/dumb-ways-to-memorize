#ifndef __ENTITY__H
#define __ENTITY__H

typedef struct vec2_s
{
	int x;
	int y;
}vec2_t;

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
	collision_type_t mCollisionType;
	entity_state_t mEntityState;
	vec2_t mAccel;
	vec2_t mVelocity;
	vec2_t mPosition;
	void (*Think)(entity_t *self);
	void (*Touch)(entity_t *self, entity_t *other, int type);
	void (*Draw)(entity_t *self);
};


#endif