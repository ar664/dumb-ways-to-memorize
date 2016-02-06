#ifndef __PARSE_FUNCTION_H
#define __PARSE_FUNCTION_H

#include "entity.h"
#include "SDL.h"


char *FunctionNames[] = {"X mouse", "X", "mouse", "use-type", "self", "at-point", "world" , 0};

extern entity_t *targEnt;
extern vec2_t mousePos;
extern vec2_t targPos;
extern SDL_GameControllerButton button;
extern int useType;


void (*ParseToFunction(const char *name))(entity_t *self);

//Functions to Parse to
void GetWorld(entity_t *self);
void GetAtPoint(entity_t *self);
void GetSelf(entity_t *self);
void GetUseType(entity_t *self);
void GetX(entity_t *self);
void GetMousePos(entity_t *self);
void GetXMouse(entity_t *self);

//Functions array
void (*FunctionSymbols[])(entity_t *self) = {GetXMouse, GetX, GetMousePos, GetUseType, GetSelf, GetAtPoint, GetWorld, 0 };

#endif