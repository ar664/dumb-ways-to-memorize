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
	INFINITE = -2,
	STATIC
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
extern power_t *gPowerUps;
extern power_t *gCurrentPowerUp;

//Power Specific
void CallInfo(power_t *self);
void UpdateNormal(power_t *power);
void UpdateInfinite(power_t *power);
int GetUseType(const char *var, int *useType);

power_t *ParseToPowerUp(object_t *power, char *str);
power_t *FindPower(char *str);
void UsePower(power_t *power);

//Interactions
void Move(entity_t *targ, entity_t *info);
void Destroy(entity_t *targ, entity_t *info);
void Spawn(entity_t *targ, entity_t *info);
void Edit(entity_t *targ, entity_t *info);
void Nullify(entity_t *targ, entity_t *info);

//Interactions Array
extern char *InteractionNames[];
extern void (*InteractionSymbols[]);

#endif