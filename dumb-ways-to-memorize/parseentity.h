#ifndef __PARSE_ENTITY_H
#define __PARSE_ENTITY_H

#include "entity.h"
#include "parseobject.h"

extern char *FileVariableNames[];		/**< The file variables which need more processing to parse */
extern char *DirectVariableNames[];		/**< The variable names which get defined by the user or predefined types and get converted to integers */
extern char *PhysicsVariableNames[];	/**< The variables which parse to vectors */
extern char *SpriteVariableNames[];		/**< The simple variables are there given values*/

#define ENTITY_SOUND_GROUP_STR	"sound_group"

typedef int (*StrConFunc)(char*);

static const size_t DirectVariableOffsets[] = 
{
	offsetof(entity_t, mSoundType),
	offsetof(entity_t, mSpriteType),
	offsetof(entity_t, mHazards),
	offsetof(entity_t, mCollisionType),
	offsetof(entity_t, mEntityState),
	offsetof(entity_t, mHealth),
	offsetof(entity_t, mDamage)
};

static const size_t SpriteVariableOffsets[] = 
{
	offsetof(sprite_t, mMillisecondsPerFrame),
	offsetof(sprite_t, mSize.x),
	offsetof(sprite_t, mSize.y),
	offsetof(sprite_t, mFrames)
};

/** Defines the enum for variables in the entity structure */
typedef enum
{
	
	ENTITY_MEMBER_SOUND,  /**<  entity sound effects, in line with animations*/
	ENTITY_MEMBER_SPRITE, /**<  sprite file names */
	ENTITY_MEMBER_FILE_MAX
}entity_members_file_t;

typedef enum
{
	ENTITY_MEMBER_SOFLAGS,			/**< Sound Type flags*/
	ENTITY_MEMBER_SPFLAGS,			/**< Sprite Type flags */
	ENTITY_MEMBER_HAZARDS,			/**< Hazard flags*/
	ENTITY_MEMBER_COLLISION_TYPE,   /**< Collision Type */
	ENTITY_MEMBER_ENTITY_STATE,		/**< State of Entity */
	ENTITY_MEMBER_HEALTH,			/**< Health of Entity */
	ENTITY_MEMBER_DAMAGE,			/**< Damage the entity deals on collision */
	ENTITY_MEMBER_DIRECT_MAX
}entity_members_direct_t;

/** Defines the enum for variables in the entity struct with vec2_t values */
typedef enum
{
	ENTITY_MEMBER_WEIGHT, /**<  weight of the entity */
	ENTITY_MEMBER_ACCEL,	/**<  Acceleratoin of entity */
	ENTITY_MEMBER_VELOCITY, /**<  Velocity of entity  */
	ENTITY_MEMBER_POSITION, /**<  Position of entity */
	ENTITY_MEMBER_VECTOR_MAX
}entity_members_physics_t;


/** Defines the enum for simple members in the entity */
typedef enum
{
	ENTITY_MEMBER_FPS,  /**<  FPS of the sprites*/
	ENTITY_MEMBER_HEIGHT, /**<  heights of the sprites */
	ENTITY_MEMBER_WIDTH,  /**<  widths of th sprites */
	ENTITY_MEMBER_FRAMES,  /**<  number of frames in each sprite */
	ENTITY_MEMBER_SIMPLE_MAX
}entity_members_sprite_t;


enum member_types
{
	MEMBER_FILE,
	MEMBER_PHYSICS,
	MEMBER_SPRITE,
	MEMBER_DIRECT
};
typedef struct entity_member_s entity_member_t;

struct entity_member_s
{
	int member_type;
	union Types
	{
		entity_members_file_t file;
		entity_members_physics_t physics;
		entity_members_sprite_t sprite;
		entity_members_direct_t direct;
	}types;
	void *data;
};

/**
 * Adds the sounds to entity to be player, to be played during their respective actions.
 * @see sound_mixer_effects
 * 
 * @param [in,out]	ent  	If non-null, the ent.
 * @param [in,out]	files	If non-null, the files.
 * @param	group		 	The group.
 *
 * @author	Anthony Rios
 * @date	4/8/2016
 */

void AddSoundsToEnt(entity_t *ent, char **files, int group);

/**
 * Loads and adds the sprite files given to the entity.
 *
 * @param [in,out]	ent  	If non-null, the ent.
 * @param [in,out]	files	If non-null, an array of the strings with file names of the sprites.
 * @param	flags		 	The flags of the entity.
 *
 * @author	Anthony Rios
 * @date	3/31/2016
 */
void AddSpritesToEnt(entity_t *ent, char **files, int flags);

/**
 * Parse to entity.
 * Tries to Assign everything , and defaults all function pointers to generic.
 * Current Frame = 0, Weight = 1
 *
 * @param [in,out]	object	If non-null, the object.
 * @param [in,out]	str   	If non-null, the string.
 *
 * @return	null if it fails, else a pointer to an entity_t.
 *
 * @author	Anthony Rios
 * @date	3/21/2016
 */
extern entity_t *ParseToEntity(object_t *object, char* str);


extern entity_member_t *GetEntityMembers(object_t *object, char *str);

#endif