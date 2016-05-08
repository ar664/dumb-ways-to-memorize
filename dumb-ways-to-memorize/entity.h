#ifndef __ENTITY__H
#define __ENTITY__H

#include "globals.h"
#include "audio.h"
#include "graphics.h"
#include "ai_interpret.h"

#define MAX_ENTITIES 250

typedef struct entity_s entity_t;
struct power_s;
struct sound_s;
struct physics_s;

/**
 * The core structure of our our entity system.
 *
 * @author	Anthony Rios
 * @date	2/17/2016
 */

struct entity_s
{
	int mHazards;					/**< The hazards that the entity is immune to / deals */
	int mHealth;					/**< The health of the entity, if below zero - destroy this entity */
	int mDamage;					/**< The damage the entity should deal to other entities */
	int mNextThink;					/**< The next time the entity should think */
	int mCurrentFrame;				/**< The current frame of animation sprite is in */
	int mNextFrameChange;			/**< The last time the frame was changed for the entity */
	//Uint8 mWeight;					/**< The weight boolean of the entity, if gravity affects him */
	Uint8 mDirection;				/**< The direction entity is facing */
	collision_type_t mCollisionType;	/**< Type of the collision the entity is */
	entity_state_t mEntityState;	/**< The state of the entity */
	struct physics_s *mPhysicsProperties;
	sprite_t **mSprites;			/**< The sprites of the given entity */
	sprite_t *mAnimation;			/**< The current animation, the entity is running */
	struct sound_s *mSounds;			/**< The sounds that belong to this entity */
	ai_function_t *mData;			/**< The data that an entity stores for its ai */
	char *mName;					/**< The name of the entity */
	//vec2_t GetAccel();					/**< The acceleration vector */
	//vec2_t GetVelocity();				/**< The velocity vector */
	void (*Think)(entity_t *self);	/**< The think function which gets called to update the entity */
	void (*Touch)(entity_t *self, entity_t *other);	/**< The function that gets called when enitities collide */
	void (*Draw)(entity_t *self);	/**< The function that gets called every frame to draw the entity */
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
 * Searches for the cached entity whose name matches given str.
 *
 * @param	name	The name of the entity.
 *
 * @return	null if it fails, else the found cached entity.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
extern entity_t *FindCachedEntity(const char *name);

/**
 * Searches for the first entity with the name of given str.
 *
 * @param	name	The name of the searched for entity.
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

/**
 * Prints the entities data, used for debug.
 *
 * @param [in,out]	ent	If non-null, the ent.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
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
 * Update the cursors position based on the SDL_GetMouseState function.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	4/26/2016
 */

void ThinkCursor(entity_t *self);


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
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void TouchGeneric(entity_t *self, entity_t *other);

/**
 * The player touch function called on collision.
 * If he touches a hazard, he gets damaged.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void TouchPlayer(entity_t *self, entity_t *other);

/**
 * The touch function for an enemy, currently does nothing.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

void TouchEnemy(entity_t *self, entity_t *other);

/**
 * The touch function for the goal entity / flag.
 * Switches gamestate based on if you won or should go to the next level.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other entity it touched.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void TouchGoal(entity_t *self, entity_t *other);

/**< The position vector, equal to the screen center position of the entity*/
vec2_t EntityPosition(entity_t *ent);
/**< The position vector, equal to on screen draw position */
vec2_t EntityDrawPosition(entity_t *ent);

extern entity_t *gEditorEntity;
//For Editor Use Only!
entity_t *NexCachedEntity();
//

extern void EntitySetAnimation(entity_t *ent, int animation);

#endif