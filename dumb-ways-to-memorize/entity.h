#ifndef __ENTITY__H
#define __ENTITY__H

#include "globals.h"
#include "graphics.h"
#include "ai_interpret.h"

#define MAX_ENTITIES 250
#define ENTITY_DIR_LEFT 0
#define ENTITY_DIR_RIGHT 1

typedef struct entity_s entity_t;
struct power_s;
/**
 * The core structure of our our entity system.
 *
 * @author	Anthony Rios
 * @date	2/17/2016
 */

struct entity_s
{
	int mHazards;					/**< The hazards that the entity is immune to / deals*/
	int mNextThink;					/**< The next time the entity should think */
	int mHealth;					/**< The health of the entity, if below zero - destroy this entity*/
	int mDamage;					/**< The damage the entity should deal to other entities */
	int mWeight;					/**< The weight boolean of the entity, if gravity affects him*/
	int mCurrentFrame;				/**< The current frame */
	int mDirection:1;				/**< The direction entity is facing*/
	collision_type_t mCollisionType;	/**< Type of the collision the entity is */
	entity_state_t mEntityState;	/**< The state of the entity */
	sprite_t **mSprites;			/**< The sprites of the given entity */
	sprite_t *mAnimation;			/**< The current animation, the entity is running */
	ai_function_t *mData;			/**< The data that an entity stores for its ai */
	char *mName;					/**< The name of the entity*/
	vec2_t mAccel;					/**< The acceleration vector */
	vec2_t mVelocity;				/**< The velocity vector */
	vec2_t mPosition;				/**< The position vector, equal to on screen draw position */
	void (*Think)(entity_t *self);	/**< The think function which gets called to update the entity*/
	void (*Touch)(entity_t *self, entity_t *other, int type);	/**< The function that gets called when enitities collide*/
	void (*Draw)(entity_t *self);	/**< The function that gets called every frame to draw the entity*/
	void (*PowerUp)(struct power_s *info);	/**< The player specific function for power_up useage */
};

extern entity_t *gEntityDictionary;
extern entity_t *gEntities;
extern int gLastEntity;

extern int InitEntitySystem();
extern entity_t *InitNewEntity();
extern entity_t *FindNextFreeCachePos();
extern entity_t *FindCachedEntity(const char *name);
extern entity_t *FindEntity(const char *name);
extern entity_t *FindFreeEntity(int* position);
extern entity_t *LookForEntityAtPos(vec2_t position);


extern int Distance2Entity(entity_t *self, entity_t *other);
extern void (*GetFunctionAI(ai_function_t *data))(entity_t *);

void RunEntities();
void DrawEntities();
extern void PrintEntity(entity_t *ent);

void FreeEntity(entity_t *ent);
void FreeNonPlayerEntities();
void ShutdownEntitySystem();

//Think & Draw Functions
void DrawGeneric(entity_t *self);
void DrawPlayer(entity_t *self);
void ThinkGeneric(entity_t *self);
void ThinkPlayer(entity_t *self);
void ThinkEnemy(entity_t *self);
void TouchGeneric(entity_t *self, entity_t *other, int type);
void TouchPlayer(entity_t *self, entity_t *other, int type);
void TouchEnemy(entity_t *self, entity_t *other, int type);
void TouchGoal(entity_t *self, entity_t *other, int type);


#endif