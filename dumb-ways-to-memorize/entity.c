#include "globals.h"
#include "entity.h"
#include "graphics.h"
#include <stdio.h>

entity_t *gEntities = NULL;
int gNumEntities = 0;

//Draw Functions
void DrawGeneric(entity_t *self)
{
	DrawSprite(self->mSprites, &self->mPosition, gRenderer);
}

void DrawPlayer(entity_t *self)
{
	DrawSprite(self->mSprites, &self->mPosition, gRenderer);
}

//Think Functions
void ThinkGeneric(entity_t *self)
{
	//Do physics
	self->Draw(self);
}

void ThinkPlayer(entity_t *self)
{
	//Do input control + physics
	self->Draw(self);
}

void ThinkEnemy(entity_t *self)
{
	//Do ai-simple, and physics
	self->Draw(self);
}


//Touch Functions
void TouchPlayer(entity_t *self, entity_t *other, int type)
{
	switch(other->mCollisionType)
	{
	case COLLISION_TYPE_STATIC:
		break;
	case COLLISION_TYPE_RAGDOLL:
		break;
	}
}

void TouchEnemy(entity_t *self, entity_t *other, int type)
{
	switch(other->mCollisionType)
	{
	case COLLISION_TYPE_STATIC:
		break;
	case COLLISION_TYPE_RAGDOLL:
		break;
	}
}

int InitEntitySystem()
{
	if( (gEntities = (entity_t*) calloc(1, sizeof(entity_t))) == NULL )
	{
		printf("Couldn't alloc EntitySys");
		return -1;
	}
	gNumEntities++;
	return 0;
}

entity_t *InitNewEntity()
{
	gEntities = (entity_t*) realloc(gEntities,sizeof(entity_t)*(gNumEntities+1));
	if(gEntities == NULL)
	{
		printf("Too many entities, could not add on more");
		exit(-1);
	}
	gNumEntities++;
	memset(&gEntities[gNumEntities], 0 ,sizeof(entity_t) );
	return &gEntities[gNumEntities-1];
}

entity_t *LookForEntity(vec2_t position)
{
	int i;
	for(i = 0; i < gNumEntities; i++)
	{
		if( (gEntities[i].mPosition.x > position.x > gEntities[i].mPosition.x + gEntities[i].mSprites->mSize.x)
			&& (gEntities[i].mPosition.y > position.y > gEntities[i].mPosition.x + gEntities[i].mSprites->mSize.y))
		{
			return &gEntities[i];
		}
	}
	return NULL;
}