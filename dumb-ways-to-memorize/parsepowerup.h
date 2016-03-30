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

/**
 * The structure for our power_up system.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */

struct power_s
{	
	char *name;											/**< The name of the power up */
	void (*UpdateInput)(power_t *self);					/**< The function to update the input for the power_up  */
	void (*GetTarg)(entity_t *self, entity_t **targ);	/**< The function which calculates who or what is the specified target */
	void (*UpdateUse)(power_t *power);					/**< The function which updates the power_up */
	void (*DoPower)(entity_t *targ, entity_t *info);	/**< The function which does the power_up , called after all the updates */
	int uses;											/**< The number of times this power can get used - @see use_type_t*/
	info_type_t info_type;								/**< The type of info the powe up needs*/
	entity_t *target;									/**< Target for the power_up */
	entity_t *info;										/**< The information that the power_up uses*/

};
extern power_t *gPowerUps;			/**< The loaded power ups */
extern power_t *gCurrentPowerUp;	/**< The currently loaded power_up */

//Power Specific
/**
 * Call information functions for the power_up.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void CallInfo(power_t *self);

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
int GetUseType(const char *var, int *useType);

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

//Interactions
/**
 * Moves the target to position given by info.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Move(entity_t *targ, entity_t *info);

/**
 * Destroys target.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Destroy(entity_t *targ, entity_t *info);

/**
 * Spawns entity from info to position of targ.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Spawn(entity_t *targ, entity_t *info);

/**
 * Edits the target with values from info.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Edit(entity_t *targ, entity_t *info);

/**
 * Nullifies the entity by removing the think function of targ.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Nullify(entity_t *targ, entity_t *info);

//Interactions Array
extern char *InteractionNames[];	/**< The interaction names, which deal with what function power does */
extern void (*InteractionSymbols[]);	/**< The interaction symbol for the functions */

#endif