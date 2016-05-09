#ifndef __PARSE_FUNCTION_H
#define __PARSE_FUNCTION_H

#include "entity.h"
#include "SDL.h"

void (*ParseToFunction(const char *name));

//Functions to Parse to
/**
 * Gets the world entities.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetWorld(entity_t *self, entity_t **targ, void *extra);

/**
 * Gets the entity at the point specified by the mouse.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetAtPoint(entity_t *self, entity_t **targ, void *extra);

/**
 * Gets self and makes it targ.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetSelf(entity_t *self, entity_t **targ, void *extra);

//Functions array
extern char *FunctionNames[];
extern void (*FunctionSymbols[]);

#endif