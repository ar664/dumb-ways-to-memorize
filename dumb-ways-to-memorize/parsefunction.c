#include "globals.h"
#include "parsefunction.h"
#include "SDL.h"
#include <string.h>
#include <stdio.h>

//Function Globals
char *FunctionNames[] = {"self", "at-point", "world" , 0};
void (*FunctionSymbols[]) = {GetSelf, GetAtPoint, GetWorld, 0 };

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

void GetWorld(entity_t *self, entity_t **targ, void *extra)
{
	*targ = gEntities;
	extra = NULL;
}

void GetAtPoint(entity_t *self, entity_t **targ, void *extra)
{
	entity_t *temp_ent;
	vec2_t temp_pos;
	vec2_t *temp_vec = (vec2_t*) malloc(sizeof(vec2_t));
	if(!temp_vec)
	{
		return;
	}
	temp_ent = FindEntity("Cursor");
	temp_pos = EntityPosition(temp_ent);
	if(extra)
	{
		memcpy(extra, &temp_pos, sizeof(vec2_t));
	}
	*targ = LookForEntityAtPos(*temp_vec);
	if(temp_vec) free(temp_vec);
}

void GetSelf(entity_t *self, entity_t **targ, void *extra)
{
	entity_t *temp_ent;
	vec2_t temp_pos;
	*targ = self;
	temp_ent = FindEntity("Cursor");
	temp_pos = EntityPosition(temp_ent);
	if(extra)
	{
		memcpy(extra, &temp_pos, sizeof(vec2_t));
	}
}