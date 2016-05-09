#include "globals.h"
#include "parsefunction.h"
#include "SDL.h"
#include <string.h>
#include <stdio.h>

//Function Globals
char *FunctionNames[] = {"X mouse", "X", "mouse", "self", "at-point", "world" , 0};
void (*FunctionSymbols[]) = {GetXMouse, GetX, GetMousePos, GetSelf, GetAtPoint, GetWorld, 0 };

void (*ParseToFunction(const char *name))
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

void GetWorld(entity_t *self, entity_t **targ)
{
	*targ = gEntities;
}

void GetAtPoint(entity_t *self, entity_t **targ)
{
	vec2_t *temp = (vec2_t*) malloc(sizeof(vec2_t));
	if(!temp)
	{
		return;
	}
	GetMousePos(self, temp);
	*targ = LookForEntityAtPos(*temp);
	if(temp) free(temp);
}

void GetSelf(entity_t *self, entity_t **targ)
{
	*targ = self;
}


void GetX(entity_t *self, int *button)
{
	*button = SDL_CONTROLLER_BUTTON_B;
}

void GetMousePos(entity_t *self, vec2_t *pos)
{
	SDL_GetRelativeMouseState( &pos->x, &pos->y);
}

void GetXMouse(entity_t *self, int * button, vec2_t *pos)
{
	GetX(self, button);
	GetMousePos(self, pos);
}