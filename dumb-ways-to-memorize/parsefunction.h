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
void GetWorld(entity_t *self, entity_t **targ);

/**
 * Gets the entity at the point specified by the mouse.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetAtPoint(entity_t *self, entity_t **targ);

/**
 * Gets self and makes it targ.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	targ	If non-null, the targ.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetSelf(entity_t *self, entity_t **targ);

/**
 * Get the status of the button of the power_up.
 *
 * @param [in,out]	self  	If non-null, the class instance that this method operates on.
 * @param [in,out]	button	If non-null, the button.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetX(entity_t *self, int * button);

/**
 * Gets mouse position.
 *
 * @param [in,out]	self	If non-null, the class instance that this method operates on.
 * @param [in,out]	pos 	If non-null, the position.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetMousePos(entity_t *self, vec2_t *pos);

/**
 * Gets status of button and mouse position.
 *
 * @param [in,out]	self  	If non-null, the class instance that this method operates on.
 * @param [in,out]	button	If non-null, the button.
 * @param [in,out]	pos   	If non-null, the position.
 *
 * @author	Anthony Rios
 * @date	3/30/2016
 */
void GetXMouse(entity_t *self, int * button, vec2_t *pos);

//Functions array
extern char *FunctionNames[];
extern void (*FunctionSymbols[]);

#endif