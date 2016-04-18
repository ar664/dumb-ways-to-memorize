#ifndef __PARSE_ENTITY_H
#define __PARSE_ENTITY_H

#include "entity.h"
#include "parseobject.h"

extern char *ComplexVariableNames[];	/**< The complex variables which need more processing to parse */
extern char *Vector2VariableNames[];	/**< The variables which parse to vectors */
extern char *SimpleVariableNames[];		/**< The simple variables are there given values*/

#define ENTITY_SOUND_GROUP_STR	"sound_group"

/** Defines the enum for variables in the entity structure */
typedef enum
{
	ENTITY_MEMBER_HAZARDS,  /**< An enum constant representing the entity member hazards */
	ENTITY_MEMBER_SOUND,  /**< An enum constant representing the entity sound effects, in line with animations*/
	ENTITY_MEMBER_COLLISION_TYPE,   /**< An enum constant representing the entity member collision type */
	ENTITY_MEMBER_ENTITY_STATE, /**< An enum constant representing the entity member entity state */
	ENTITY_MEMBER_MAX
}entity_members_complex_t;

/** Defines the enum for variables in the entity struct with vec2_t values */
typedef enum
{
	ENTITY_MEMBER_ACCEL,	/**< An enum constant representing the entity member accel  */
	ENTITY_MEMBER_VELOCITY, /**< An enum constant representing the entity member velocity  */
	ENTITY_MEMBER_POSITION, /**< An enum constant representing the entity member position */
}entity_members_vector2_t;


/** Defines the enum for simple members in the entity */
typedef enum
{
	ENTITY_MEMBER_SPRITE, /**< An enum constant representing the sprite file names */
	ENTITY_MEMBER_HEALTH, /**< An enum constant representing the default health of the entity */
	ENTITY_MEMBER_FPS,  /**< An enum constant representing the FPS of the sprites*/
	ENTITY_MEMBER_DAMAGE, /**< An enum constant representing the damage the entity deals on collision */
	ENTITY_MEMBER_HEIGHT, /**< An enum constant representing the heights of the sprites */
	ENTITY_MEMBER_WIDTH,  /**< An enum constant representing the widths of th sprites */
	ENTITY_MEMBER_FRAMES  /**< An enum constant representing the number of frames in each sprite */
}entity_members_simple_t;

/**
 * Alias for a switch case that handles complex member to entity.
 *
 * @param [in,out]	ent  	If non-null, the ent.
 * @param	member		 	The member.
 * @param [in,out]	value	If non-null, the value.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void AddComplexMemToEnt(entity_t *ent, entity_members_complex_t member, void *value);

/**
 * Handles entering / converting complex members into the entity given.
 *
 * @param [in,out]	ent  	If non-null, the ent.
 * @param [in,out]	token	If non-null, the token(s) to parse.
 * @param [in,out]	str  	If non-null, the global string to parse tokens from.
 * @param	member		 	The complex member associated with values given.
 * @param	size		 	The amount of values to input.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
void ParseComplexMember(entity_t *ent, jsmntok_t* token, char *str, entity_members_complex_t member, int size);

/**
 * Adds a vector2 to the entity, which is a switch case similar to AddComplexMemToEnt.
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
 * @param	size		 	The size.
 *
 * @author	Anthony Rios
 * @date	3/31/2016
 */
void AddSpritesToEnt(entity_t *ent, char **files, int size);

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


#endif