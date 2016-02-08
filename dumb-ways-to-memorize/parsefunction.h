#ifndef __PARSE_FUNCTION_H
#define __PARSE_FUNCTION_H

#include "entity.h"
#include "SDL.h"

void (*ParseToFunction(const char *name));

//Functions to Parse to
void GetWorld(entity_t *self, entity_t **targ);
void GetAtPoint(entity_t *self, entity_t **targ);
void GetSelf(entity_t *self, entity_t **targ);
void GetX(entity_t *self, int * button);
void GetMousePos(entity_t *self, vec2_t *pos);
void GetXMouse(entity_t *self, int * button, vec2_t *pos);

//Functions array
extern char *FunctionNames[];
extern void (*FunctionSymbols[]);

#endif