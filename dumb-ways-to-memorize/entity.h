#ifndef __ENTITY__H
#define __ENTITY__H

#include "globals.h"
#include "graphics.h"
#include "ai_interpret.h"

#define MAX_ENTITIES 750
#define ENTITY_DIR_LEFT 0
#define ENTITY_DIR_RIGHT 1

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
	int mNextThink;
	int mHealth;
	int mDamage;
	int mWeight;
	int mDirection:1;				/**< The direction entity is facing*/
	collision_type_t mCollisionType;
	entity_state_t mEntityState;
	sprite_t **mSprites;
	sprite_t *mAnimation;			/**< The animation */
	ai_function_t *mData;
	char *mName;
	vec2_t mAccel;
	vec2_t mVelocity;
	vec2_t mPosition;
	void (*Think)(entity_t *self);
	void (*Touch)(entity_t *self, entity_t *other, int type);
	void (*Draw)(entity_t *self);
	void (*PowerUp)(entity_t *self);
};

extern entity_t *gEntityDictionary;
extern entity_t *gEntities;
extern int gLastEntity;

extern int InitEntitySystem();
extern entity_t *InitNewEntity();
extern entity_t *FindCachedEntity(const char *name);
extern entity_t *FindEntity(const char *name);
extern entity_t *FindFreeEntity(int* position);
extern entity_t *LookForEntityAtPos(vec2_t position);

extern int Distance2Entity(entity_t *self, entity_t *other);
extern void (*GetFunctionAI(ai_function_t *data))(entity_t *);

void RunEntities();
void DrawEntities();

void FreeEntity(entity_t *ent);
void FreeNonPlayerEntities();
void ShutdownEntitySystem();

#endif