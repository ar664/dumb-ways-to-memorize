#include "globals.h"
#include "parsefunction.h"
#include "SDL.h"
#include <string.h>

//Global
entity_t *targEnt = NULL;
vec2_t mousePos = {0,0};
vec2_t targPos = {0,0};
SDL_GameControllerButton button;
int useType = -1;

void *ParseToVariable(const char *name)
{
	return NULL;
}

void (*ParseToFunction(const char *name))(entity_t *self)
{
	int i;

	for(i = 0; FunctionNames[i]; i++)
	{
		if(!strcmp(FunctionNames[i], name))
		{
			return FunctionSymbols[i];
		}
	}
	return NULL;
}

void GetWorld(entity_t *self)
{
	targEnt = gEntities;
	return;
}

void GetAtPoint(entity_t *self)
{
	GetMousePos(self);
	targEnt = LookForEntity(mousePos);
	targPos = mousePos;
	return;
}

void GetSelf(entity_t *self)
{
	targEnt = self;
	return;
}

void GetUseType(entity_t *self)
{
	self->PowerUp(self);
	return;
}

void GetX(entity_t *self)
{
	button = SDL_GameControllerGetButtonFromString("X");
	return;
}

void GetMousePos(entity_t *self)
{
	SDL_GetRelativeMouseState(&mousePos.x, &mousePos.y );
	return;
}

void GetXMouse(entity_t *self)
{
	GetX(self);
	GetMousePos(self);
}