#include "globals.h"
#include "entity.h"
#include "graphics.h"
#include <stdio.h>

entity_t *gEntities = NULL;
int gLastEntity = 0;
char **Hazards_str = NULL;
char *Collisions_str[] = {"static", "ragdoll", "clip", 0};
char *EntityStates_str[] = {"alive", "dead", "other", 0};

//Draw Functions
void DrawGeneric(entity_t *self)
{
	//DrawSprite(self->mSprites, &self->mPosition, gRenderer);
}

void DrawPlayer(entity_t *self)
{
	//DrawSprite(self->mSprites, &self->mPosition, gRenderer);
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
	default:
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
	default:
		break;
	}
}

int InitEntitySystem()
{
	if( (gEntities = (entity_t*) malloc(sizeof(entity_t)*MAX_ENTITIES)) == NULL )
	{
		printf("Couldn't alloc EntitySys");
		return -1;
	}
	memset(gEntities, 0, sizeof(entity_t)*MAX_ENTITIES);
	gLastEntity = 0;
	return 0;
}

entity_t *InitNewEntity()
{
	int pos;
	entity_t *retVal;
	if(gEntities == NULL)
	{
		printf("Entity system unintialiazeed");
		exit(-1);
	}

	retVal = FindFreeEntity(&pos);
	gLastEntity = pos;
	return retVal;
}

entity_t* FindCachedEntity(const char* name)
{
	int i;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntityDictionary[i].mName)
			break;
		if(!strcmp(name, gEntityDictionary[i].mName))
		{
			return &gEntities[i];
		}
	}
	return NULL;
}

entity_t* FindEntity(const char* name)
{
	int i;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
			continue;
		if(!strcmp(name, gEntities[i].mName))
		{
			return &gEntities[i];
		}
	}
	return NULL;
}

entity_t* FindFreeEntity(int* position)
{
	int i;
	for(i = gLastEntity; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
		{
			if(position)
				*position = i;
			return &gEntities[i];
		}
	}
	for(i = 0; i < gLastEntity; i++)
	{
		if(!gEntities[i].mName)
		{
			if(position)
				*position = i;
			return &gEntities[i];
		}
	}
	return NULL;
}


entity_t *LookForEntityAtPos(vec2_t position)
{
	int i;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
			continue;
		if( (gEntities[i].mPosition.x > position.x > gEntities[i].mPosition.x + gEntities[i].mSprites[0]->mSize.x)
			&& (gEntities[i].mPosition.y > position.y > gEntities[i].mPosition.x + gEntities[i].mSprites[0]->mSize.y))
		{
			return &gEntities[i];
		}
	}
	return NULL;
}

void FreeEntity(entity_t *ent)
{
	int i;
	if(!ent)
		return;
	i = 0;
	if(ent->mSprites)
	{
		while(ent->mSprites[i])
		{
			FreeSprite(ent->mSprites[i]);
			i++;
		}
		free(ent->mSprites);
	}
	if(ent->mName)
	{
		free(ent->mName);
	}
	memset(ent, 0, sizeof(entity_t));

}

void ShutdownEntitySystem()
{
	int i;
	if(!gEntities)
		return;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
			continue;
		FreeEntity(&gEntities[i]);
	}
	free(gEntities);
}

int StrToHazard(char *str)
{
	int i, length;
	if(!str)
		return 0;
	length = CountMem(Hazards_str, sizeof(char*));
	for(i = 0; i < length; i++)
	{
		if(!strcmp(str, Hazards_str[i]))
		{
			return (1 << i);
		}
	}
	return 0;
}

collision_type_t StrToCollisionType(char *str)
{
	int i;
	if(!str)
		return 0;
	for(i = 0; i <= COLLISION_TYPE_CLIP; i++)
	{
		if(!strcmp(Collisions_str[i], str))
		{
			return (collision_type_t)i;
		}
	}
	return 0;
}

entity_state_t StrToEntityState(char *str)
{
	int i;
	if(!str)
		return 0;
	for(i = 0; i <= COLLISION_TYPE_CLIP; i++)
	{
		if(!strcmp(EntityStates_str[i], str))
		{
			return (entity_state_t)i;
		}
	}
	return 0;
}