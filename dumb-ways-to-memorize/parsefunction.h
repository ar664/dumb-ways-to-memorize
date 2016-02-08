#ifndef __PARSE_FUNCTION_H
#define __PARSE_FUNCTION_H

#include "entity.h"
#include "SDL.h"


extern char *FunctionNames[];

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
void GetX(entity_t *self);
void GetMousePos(entity_t *self);
void GetXMouse(entity_t *self);
void UseTypePlaceHolder(entity_t *self);
int GetUseType(const char *var);

//Functions array
extern void (*FunctionSymbols[])(entity_t *self);

#endif