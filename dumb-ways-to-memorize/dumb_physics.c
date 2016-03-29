#include "dumb_physics.h"
#include "entity.h"
#include "globals.h"
#include "player.h"
#include <stdio.h>

vec2_t gGravity = {0, 2};

/**
 * Executes physics.
 * RunTime O( n(n+1)/2 )
 *
 * @author	Anthony Rios
 * @date	3/20/2016
 */

void RunPhysics()
{
	int i, j;
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
		{
			continue;
		}
		if(!gEntities[i].mCollisionType == COLLISION_TYPE_STATIC)
		{
			Vec2Add(&gEntities[i].mVelocity,&gEntities[i].mPosition,&gEntities[i].mPosition);
			Vec2Add(&gEntities[i].mAccel,&gEntities[i].mVelocity,&gEntities[i].mVelocity);
			if(gEntities[i].mWeight)
			{
				gEntities[i].mAccel.y = gGravity.y;
			}
			ApplySpeedLimit(&gEntities[i].mVelocity);
			ApplySpeedLimit(&gEntities[i].mAccel);
			ApplyBounds(&gEntities[i]);
			ApplyFriction(&gEntities[i].mVelocity);
		}
		
		//Vec2Add(&friction,&gEntities[i].mVelocity,&gEntities[i].mVelocity);
		
	}
	//Collision Check
	for(i = 0; i < MAX_ENTITIES; i++)
	{
		if(!gEntities[i].mName)
		{
			continue;
		}
		for(j = i; j < MAX_ENTITIES; j++)
		{
			if(i == j || !gEntities[j].mName)
			{
				continue;
			}
			if(CheckCollision(&gEntities[i], &gEntities[j]))
			{
				DoCollision(&gEntities[i], &gEntities[j]);
			}
		}
	}
}

/**
 * Check collision between entities.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 *
 * @return	An int 1 if colliding, 0 if not.
 *
 * @author	Anthony Rios
 * @date	3/20/2016
 */

int CheckCollision(entity_t *self, entity_t *other)
{
	if(self->mCollisionType == COLLISION_TYPE_CLIP || other->mCollisionType == COLLISION_TYPE_CLIP)
	{
		return 0;
	}
	if(!self->mSprites || !self->mSprites)
	{
		return 0;
	}
	if(self->mPosition.x + self->mSprites[0]->mSize.x < other->mPosition.x || self->mPosition.y + self->mSprites[0]->mSize.y < other->mPosition.y
		|| self->mPosition.x > other->mPosition.x + other->mSprites[0]->mSize.x || self->mPosition.y > other->mPosition.y + other->mSprites[0]->mSize.y)
	{
		return 0;
	}
	return 1;
}

/**
 * Executes the collision operation.
 *
 * @param [in,out]	self 	If non-null, the class instance that this method operates on.
 * @param [in,out]	other	If non-null, the other.
 *
 * @author	Anthony Rios
 * @date	3/20/2016
 */

void DoCollision(entity_t *self, entity_t *other)
{
	vec2_t self_min, self_max, self_res, other_min, other_max, other_res;
	int left, right, bottom, top;
	if(self->Touch)
	{
		self->Touch(self, other, other->mCollisionType);
	}
	if(other->Touch)
	{
		other->Touch(other, self, self->mCollisionType);
	}
	if(strcmp(self->mName, other->mName))
	{
		printf("%s collided with %s \n", self->mName, other->mName);
	}
	self_min = self->mPosition;
	Vec2Add(&self->mPosition, &self->mSprites[0]->mSize, &self_max);
	other_min = other->mPosition;
	Vec2Add(&other->mPosition, &other->mSprites[0]->mSize, &other_max);

	left = other_min.x - self_max.x;
	right = other_max.x - self_min.x;
	top = other_min.y - other_max.y;
	bottom = other_max.y - self_max.y;
	if(abs(left) < right)
	{
		self_res.x = left;
		other_res.x = right;
	} else
	{
		self_res.x = right;
		other_res.x = left;
	}

	if(abs(top) < bottom)
	{
		self_res.y = top;
		other_res.y = bottom;
	} else
	{
		self_res.y = bottom;
		other_res.y = top;
	}
	
	if(abs(self_res.x) < abs(self_res.y))
	{
		self_res.y = 0;
	} else
	{
		self_res.x = 0;
	}

	if(abs(other_res.x) < abs(other_res.y))
	{
		other_res.y = 0;
	} else
	{
		other_res.x = 0;
	}

	if(self->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		Vec2Add(&self->mPosition, &other_res, &self->mPosition);
		if(other_res.y)
		{
			self->mVelocity.y = -self->mVelocity.y/2;
		} else
		{
			self->mVelocity.x = -self->mVelocity.x/2;
		}
	}
	if(other->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		Vec2Add(&other->mPosition, &self_res, &other->mPosition);
		if(self_res.y)
		{
			other->mVelocity.y = -self->mVelocity.y/2;
		} else
		{
			other->mVelocity.x = -self->mVelocity.x/2;
		}

	}

}

void ApplySpeedLimit(vec2_t* a)
{
	a->x = abs(a->x) > PHYSICS_MAX_SPEED ? (a->x < 0 ? -PHYSICS_MAX_SPEED : PHYSICS_MAX_SPEED) : a->x;
	a->y = abs(a->y) > PHYSICS_MAX_SPEED ? (a->y < 0 ? -PHYSICS_MAX_SPEED : PHYSICS_MAX_SPEED) : a->y;
}

void ApplyBounds(entity_t* ent)
{
	if(ent->mPosition.x < 0)
	{
		ent->mPosition.x = 0;
		ent->mVelocity.x = 0;
		ent->mAccel.x = 0;
	} else if (ent->mPosition.x > gScreenWidth)
	{
		ent->mPosition.x = gScreenWidth - PHYSICS_MAX_SPEED;
		ent->mVelocity.x = 0;
		ent->mAccel.x = 0;
	}
	if(ent->mPosition.y < 0)
	{
		ent->mPosition.y = 0;
		ent->mVelocity.y = 0;
		ent->mAccel.y = 0;
	} else if (ent->mPosition.y > gScreenHeight)
	{
		ent->mPosition.y = gScreenHeight - PHYSICS_MAX_SPEED;
		ent->mVelocity.y = 0;
		ent->mAccel.y = 0;
	}
}

void ApplyFriction(vec2_t* a)
{
	if(abs(a->x))
	{
		a->x += a->x < 0 ? PHYSICS_BASE_FRICTION : -PHYSICS_BASE_FRICTION;
	}
	if(abs(a->y))
	{
		a->y += a->y < 0 ? PHYSICS_BASE_FRICTION : -PHYSICS_BASE_FRICTION;
	}
}
