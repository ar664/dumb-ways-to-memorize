#ifndef __PARSE_POWER_UP_H
#define __PARSE_POWER_UP_H

#include "parseobject.h"
#include "entity.h"

#define POWER_TARGET_STR "target"
#define POWER_USE_TYPE_STR "use-type"
#define POWER_INTERACTION_STR "interaction"
#define POWER_INPUT_TYPE_STR "input-type"
#define POWER_ENTITY_STR "entity"

/** Values that represent use types for power_ups */
enum use_type_t
{
	INFINITE = -2,  /**< An enum constant representing infinite use of a power_up, which implies activation prior to use */
	STATIC			/**< An enum constant representing static use of a power_up, which implies passive activation */
};

/** Defines an alias representing the types of info the power_up needs */
typedef enum 
{
	INFO_NONE	= 0x0,
	INFO_BUTTON = 0x1,
	INFO_MOUSE	= 0x2,
	INFO_BOTH	= INFO_MOUSE | INFO_BUTTON

}info_type_t;

typedef struct power_s power_t;

typedef void (*PowerFunc) (entity_t*, entity_t**, void *extra);

/**
 * The structure for our power_up system.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

struct power_s
{	
	char *name;											/**< The name of the power up */
	sprite_t *icon;										/**< Icon of the power up */
	void *extra;										/**< The extra var to use when using power up, allocated on load */
	void (*IterateThroughTargets)(power_t *power);		/**< The function which calculates who or what is the specified target, and iterates through them */
	void (*UpdateUse)(power_t *power);					/**< The function which updates the power_up */
	PowerFunc DoPower;									/**< The function which does the power_up , called during iterate through targets */
	int uses;											/**< The number of times this power can get used - @see use_type_t*/
	entity_t *target;									/**< Target for the power_up */
	entity_t *info;										/**< The information that the power_up uses*/

};
extern power_t *gPowerUps;			/**< The loaded power ups */
extern power_t *gCurrentPowerUp;	/**< The currently loaded power_up */


/**
 * Updates a normal use power_up, with uses > 0.
 *
 * @param [in,out]	power	If non-null, the power.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void UpdateNormal(power_t *power);

/**
 * Updates an infinite use power_up
 *
 * @param [in,out]	power	If non-null, the power.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void UpdateInfinite(power_t *power);

/**
 * Gets the usetype from var, and puts it in useType.
 *
 * @param	var			   	The variable.
 * @param [in,out]	useType	If non-null, type of the use.
 *
 * @return	The use type.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
int GetUseType(power_t *power, const char *useType);

/**
 * Parses object/str data to a power up.
 *
 * @param [in,out]	power	If non-null, the power.
 * @param [in,out]	str  	If non-null, the string.
 *
 * @return	null if it fails, else a pointer to a power_t.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
power_t *ParseToPowerUp(object_t *power, char *str);

/**
 * Searches for the first power with matching name str.
 *
 * @param [in,out]	str	If non-null, the string.
 *
 * @return	null if it fails, else the found power.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
power_t *FindPower(char *str);

/**
 * Uses the power, more like a macro then anything.
 *
 * @param [in,out]	power	If non-null, the power.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void UsePower(power_t *power);

void (*ParseToIterator( char *str));

/**
 * Gets the world entities.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void IterateThroughWorld(power_t *power);

/**
 * Gets the entity at the point specified by the mouse.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void IterateAtPoint(power_t *power);

/**
 * Gets self and makes it targ.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void IterateSelf(power_t *power);

//Interactions Array
extern char *IterationNames[];	/**< The interaction names, which deal with what function power does */
extern void (*IterationSymbols[]);	/**< The interaction symbol for the functions */

#endif