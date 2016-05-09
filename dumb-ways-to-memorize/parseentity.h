#ifndef __PARSE_ENTITY_H
#define __PARSE_ENTITY_H

#include "entity.h"
#include "parseobject.h"

extern char *FileVariableNames[];		/**< The file variables which need more processing to parse */
extern char *DynamicVariableNames[];		/**< The variable names which get defined by the user or predefined types and get converted to integers */
extern char *Vector2VariableNames[];	/**< The variables which parse to vectors */
extern char *SimpleVariableNames[];		/**< The simple variables are there given values*/

#define ENTITY_SOUND_GROUP_STR	"sound_group"

/** Defines the enum for variables in the entity structure */
typedef enum
{
	
	ENTITY_MEMBER_SOUND,  /**< An enum constant representing the entity sound effects, in line with animations*/
	ENTITY_MEMBER_SPRITE, /**< An enum constant representing the sprite file names */
	ENTITY_MEMBER_FILE_MAX
}entity_members_file_t;

typedef enum
{
	ENTITY_MEMBER_HAZARDS,			/**< An enum constant representing the entity member hazards */
	ENTITY_MEMBER_COLLISION_TYPE,   /**< An enum constant representing the entity member collision type */
	ENTITY_MEMBER_ENTITY_STATE,		/**< An enum constant representing the entity member entity state */
	ENTITY_MEMBER_DYNAMIC_MAX
}entity_members_dynamic_t;

/** Defines the enum for variables in the entity struct with vec2_t values */
typedef enum
{
	ENTITY_MEMBER_ACCEL,	/**< An enum constant representing the entity member accel  */
	ENTITY_MEMBER_VELOCITY, /**< An enum constant representing the entity member velocity  */
	ENTITY_MEMBER_POSITION, /**< An enum constant representing the entity member position */
	ENTITY_MEMBER_VECTOR_MAX
}entity_members_vector2_t;


/** Defines the enum for simple members in the entity */
typedef enum
{
	ENTITY_MEMBER_HEALTH, /**< An enum constant representing the default health of the entity */
	ENTITY_MEMBER_WEIGHT, /**< An enum constant representing the weight of the entity */
	ENTITY_MEMBER_FPS,  /**< An enum constant representing the FPS of the sprites*/
	ENTITY_MEMBER_DAMAGE, /**< An enum constant representing the damage the entity deals on collision */
	ENTITY_MEMBER_SFLAGS,  /**< An enum constant representing the drawing flags */
	ENTITY_MEMBER_HEIGHT, /**< An enum constant representing the heights of the sprites */
	ENTITY_MEMBER_WIDTH,  /**< An enum constant representing the widths of th sprites */
	ENTITY_MEMBER_FRAMES,  /**< An enum constant representing the number of frames in each sprite */
	ENTITY_MEMBER_SIMPLE_MAX
}entity_members_simple_t;


enum member_types
{
	MEMBER_COMPLEX,
	MEMBER_VECTOR,
	MEMBER_SIMPLE
};
typedef struct entity_member_s entity_member_t;

struct entity_member_s
{
	int member_type;
	union Types
	{
		entity_members_file_t complex;
		entity_members_vector2_t vector;
		entity_members_simple_t simple;
	}types;
	void *data;
};

/**
 * Adds a vector2 to the entity. Called after physics vars given.
 *
 * @param [in,out]	ent  	If non-null, the ent.
 * @param	member		 	The vector member to enters.
 * @param [in,out]	value	If non-null, the value of the vector.
 *
 * @author	Anthony Rios
 * @date	3/31/2016
 */
void AddVector2Entity(entity_t *ent, entity_members_vector2_t member, vec2_t *value);

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