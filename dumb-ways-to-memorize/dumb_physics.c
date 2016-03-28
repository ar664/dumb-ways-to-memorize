#include "dumb_physics.h"
#include "entity.h"
#include "globals.h"
#include "player.h"

vec2_t gGravity = {0, 9};

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
				Vec2Add(&gGravity,&gEntities[i].mAccel,&gEntities[i].mAccel);
			}
			ApplySpeedLimit(&gEntities[i].mVelocity);
			ApplySpeedLimit(&gEntities[i].mAccel);
			ApplyBounds(&gEntities[i].mPosition);
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
	if(self->mPosition.x + self->mSprites[0]->mSize.x >= other->mPosition.x && self->mPosition.x <= other->mPosition.x + other->mSprites[0]->mSize.x)
	{
		if(self->mPosition.y + self->mSprites[0]->mSize.y >= other->mPosition.y && self->mPosition.y <= other->mPosition.y + other->mSprites[0]->mSize.y)
		{
			return 1;
		}
	}
	return 0;
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
	vec2_t position_self, position_other, knockback_self = {0}, knockback_other = {0};
	if(self->Touch)
	{
		self->Touch(self, other, other->mCollisionType);
	}
	if(other->Touch)
	{
		other->Touch(other, self, self->mCollisionType);
	}
	
	if(self->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		position_self.x = self->mPosition.x < (other->mPosition.x + other->mSprites[0]->mSize.x) ? self->mPosition.x - other->mPosition.x  : 0;
		position_self.y = self->mPosition.y < (other->mPosition.y + other->mSprites[0]->mSize.y) ? self->mPosition.y - other->mPosition.y : 0;
	} else
	{
		position_self = gZeroPos;
	}
	if(other->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		position_other.x = other->mPosition.x < (self->mPosition.x + self->mSprites[0]->mSize.x) ? other->mPosition.x - self->mPosition.x  : 0;
		position_other.y = other->mPosition.y < (self->mPosition.y + self->mSprites[0]->mSize.y) ? other->mPosition.y - self->mPosition.y : 0;	
	} else
	{
		position_other = gZeroPos;
	}
	(position_self.x > position_self.y) ? (knockback_self.y = 0 ): (knockback_self.x = 0 );
	(position_other.x > position_other.y) ? (knockback_other.y = 0 ): (knockback_other.x = 0 );

	if(self->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		Vec2Add(&position_self, &self->mPosition, &self->mPosition);
		Vec2Add(&knockback_self, &self->mVelocity, &self->mVelocity);
		self->mAccel.x = 0;
		self->mAccel.y = 0;
	}
	if(other->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		Vec2Add(&position_other, &other->mPosition, &other->mPosition);
		Vec2Add(&knockback_other, &other->mVelocity, &other->mVelocity);
		self->mAccel.x = 0;
		self->mAccel.y = 0;
	}
}

void ApplySpeedLimit(vec2_t* a)
{
	a->x = abs(a->x) > PHYSICS_MAX_SPEED ? (a->x < 0 ? -PHYSICS_MAX_SPEED : PHYSICS_MAX_SPEED) : a->x;
	a->y = abs(a->y) > PHYSICS_MAX_SPEED ? (a->y < 0 ? -PHYSICS_MAX_SPEED : PHYSICS_MAX_SPEED) : a->y;
}

void ApplyBounds(vec2_t* a)
{
	if(a->x < 0)
	{
		a->x = 0;
	} else if (a->x > gScreenWidth)
	{
		a->x = gScreenWidth - PHYSICS_MAX_SPEED;
	}
	if(a->y < 0)
	{
		a->y = 0;
	} else if (a->y > gScreenHeight)
	{
		a->y = gScreenHeight - PHYSICS_MAX_SPEED;
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
