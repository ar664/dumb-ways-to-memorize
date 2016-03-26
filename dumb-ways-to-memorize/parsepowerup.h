#ifndef __PARSE_POWER_UP_H
#define __PARSE_POWER_UP_H

#include "parseobject.h"
#include "entity.h"

#define POWER_TARGET_STR "target"
#define POWER_USE_TYPE_STR "use-type"
#define POWER_INPUT_TYPE_STR "input-type"
#define POWER_ENTITY_STR "entity"

enum use_type_t
{
	INFINITE = -2,
	STATIC
};

typedef enum 
{
	INFO_NONE	= 0x0,
	INFO_BUTTON = 0x1,
	INFO_MOUSE	= 0x2,
	INFO_BOTH	= INFO_MOUSE | INFO_BUTTON

}info_type_t;

typedef struct power_s power_t;

struct power_s
{	
	char *name;
	void (*UpdateInput)(power_t *self);
	void (*GetTarg)(entity_t *self, entity_t **targ);
	void (*UpdateUse)(power_t *power);
	void (*DoPower)(entity_t *targ, entity_t *info);
	int uses;
	info_type_t info_type;
	entity_t *target;
	entity_t *info;

};
extern power_t *gPowerUps;

//Power Specific
void CallInfo(power_t *self);
void UpdateNormal(power_t *power);
void UpdateInfinite(power_t *power);
int GetUseType(const char *var, int *useType);

power_t *ParseToPowerUp(object_t *power, char *str);
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