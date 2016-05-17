#ifndef __PARSE_FUNCTION_H
#define __PARSE_FUNCTION_H

#include "entity.h"
#include "SDL.h"

#define SPAWN_OFFSET 10

void (*ParseToFunction(const char *name));

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
void Move(entity_t *targ, entity_t **info, void *extra);

/**
 * Destroys target.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Destroy(entity_t *targ, entity_t **info, void *extra);

/**
 * Spawns entity from info to position of targ.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Spawn(entity_t *targ, entity_t **info, void *extra);

/**
 * Edits the target with values from info.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Edit(entity_t *targ, entity_t **info, void *extra);

/**
 * Nullifies the entity by removing the think function of targ.
 *
 * @param [in,out]	targ	If non-null, the targ.
 * @param [in,out]	info	If non-null, the information.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void Nullify(entity_t *targ, entity_t **info, void *extra);


//Functions array
extern char *FunctionNames[];
extern void (*FunctionSymbols[]);

#endif