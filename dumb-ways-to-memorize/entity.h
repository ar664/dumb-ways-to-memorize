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
	char *mName;					/**< The gPlayerName of the entity*/
	vec2_t mAccel;					/**< The acceleration vector */
	vec2_t mVelocity;				/**< The velocity vector */
	vec2_t mPosition;				/**< The position vector, equal to on screen draw position */
	void (*Think)(entity_t *self);	/**< The think function which gets called to update the entity*/
	void (*Touch)(entity_t *self, entity_t *other, int type);	/**< The function that gets called when enitities collide*/
	void (*Draw)(entity_t *self);	/**< The function that gets called every frame to draw the entity*/
	void (*PowerUp)(struct power_s *info);	/**< The player specific function for power_up useage */
};

extern entity_t *gEntityDictionary;	/**< Entities loaded from files AKA cached entities */
extern entity_t *gEntities;			/**< The memory for entities that are currently running in the game */
extern int gLastEntity;				/**< The last entity that was free'd or created */

/**
 * Init entity system.
 *
 * @return	An int of failure state. 0 if good, -1 if error.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern int InitEntitySystem();

/**
 * Init new entity in the memory of global entity space.
 *
 * @return	null if it fails, else a pointer to an entity_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern entity_t *InitNewEntity();

/**
 * Searches for the next free cache position in gEntitiesDictionary.
 *
 * @return	null if it fails, else the found free cache position.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern entity_t *FindNextFreeCachePos();

/**
 * Searches for the cached entity whose gPlayerName matches given str.
 *
 * @param	gPlayerName	The gPlayerName.
 *
 * @return	null if it fails, else the found cached entity.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern entity_t *FindCachedEntity(const char *name);

/**
 * Searches for the first entity with the gPlayerName of given str.
 *
 * @param	gPlayerName	The gPlayerName.
 *
 * @return	null if it fails, else the found entity.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern entity_t *FindEntity(const char *name);

/**
 * Searches for the first free entity.
 *
 * @param [in,out]	position	If non-null, the position.
 *
 * @return	null if it fails, else the found free entity.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern entity_t *FindFreeEntity(int* position);

/**
 * Look for entity at position.
 *
 * @param	position	The position.
 *
 * @return	null if it fails, else a pointer to an entity_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern entity_t *LookForEntityAtPos(vec2_t position);

/**
 * Distance 2 entity other from entity self.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other entity.
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern int Distance2Entity(entity_t *self, entity_t *other);

/**
 * Returns a function pointer to the think function, given ai_function data.
 *
 * @param [in,out]	parameter1	If non-null, the first parameter.
 *
 * @return	null if it fails, else a GetFunctionAI(ai_function_t *data.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern void (*GetFunctionAI(ai_function_t *data))(entity_t *);

/**
 * Executes the entities think functions.
 *
 * @note	I think I have the logic wrong..
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void RunEntities();

/**
 * Draw entities, if they have draw functions.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DrawEntities();
extern void PrintEntity(entity_t *ent);

/**
 * Free entity.
 * 
 * @note	right now it just acts like memset(0) on entities in the global Entities scope.
 * 			But it free's any other data that isn't globally set.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void FreeEntity(entity_t *ent);

/**
 * Frees the non player entities, use for Destroy World.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void FreeNonPlayerEntities();

/**
 * Shutdown entity system, frees all the entities.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ShutdownEntitySystem();

//Draw Functions
/**
 * Draws the entity via self->mAnimation if set, or the first sprite which is idle.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void DrawGeneric(entity_t *self);
//Unused for now
void DrawPlayer(entity_t *self);

//Think Functions
/**
 * Generic think function, checks health and dies when health <= 0
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ThinkGeneric(entity_t *self);

/**
 * The player think function, handles lives ,gamestate switching , and input.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ThinkPlayer(entity_t *self);

/**
 * The modular think function for enemy entities with an AI.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ThinkEnemy(entity_t *self);

/**
 * Generic touch function,
 * Receives pain from touch and inflict pain on touch depending on hazard types.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 * @param	type		 	The type.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void TouchGeneric(entity_t *self, entity_t *other, int type);

/**
 * The player touch function called on collision.
 * If he touches a hazard, he gets damaged.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 * @param	type		 	The type.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void TouchPlayer(entity_t *self, entity_t *other, int type);

/**
 * The touch function for an enemy, currently does nothing.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 * @param	type		 	The type.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

void TouchEnemy(entity_t *self, entity_t *other, int type);

/**
 * The touch function for the goal entity / flag.
 * Switches gamestate based on if you won or should go to the next level.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other entity it touched.
 * @param	type		 	The type.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void TouchGoal(entity_t *self, entity_t *other, int type);


#endif