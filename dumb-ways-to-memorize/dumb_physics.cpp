#include "dumb_physics.h"
#include "entity.h"

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
	vec2_t gravity, friction;
	gravity.x = 0; gravity.y = 9;
	friction.x = 0; friction.y = 1;
	for(i = 0; gEntities[i].mName; i++)
	{
		Vec2Add(&gEntities[i].mVelocity,&gEntities[i].mPosition,&gEntities[i].mPosition);
		Vec2Add(&gEntities[i].mAccel,&gEntities[i].mVelocity,&gEntities[i].mVelocity);
		Vec2Add(&gravity,&gEntities[i].mAccel,&gEntities[i].mAccel);
		Vec2Add(&friction,&gEntities[i].mVelocity,&gEntities[i].mVelocity);
		
	}
	//Collision Check
	for(i = 0; gEntities[i].mName; i++)
	{
		for(j = i; gEntities[j].mName; j++)
		{
			if(i == j)
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
 * @return	An int.
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
	vec2_t position_self, position_other;
	if(self->Touch)
	{
		self->Touch(self, other, other->mCollisionType);
	} else if(other->Touch)
	{
		other->Touch(other, self, self->mCollisionType);
	}
	
	if(self->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		position_self.x = self->mPosition.x < (other->mPosition.x + other->mSprites[0]->mSize.x) ? other->mPosition.x + other->mSprites[0]->mSize.x : other->mPosition.x;
		position_self.y = self->mPosition.y > (other->mPosition.y + other->mSprites[0]->mSize.y) ? other->mPosition.y + other->mSprites[0]->mSize.y : other->mPosition.y;
	} else
	{
		position_self = self->mPosition;
	}
	if(other->mCollisionType == COLLISION_TYPE_RAGDOLL)
	{
		position_other.x = other->mPosition.x < (self->mPosition.x + self->mSprites[0]->mSize.x) ? self->mPosition.x + self->mSprites[0]->mSize.x : self->mPosition.x;
		position_other.y = other->mPosition.y > (self->mPosition.y + self->mSprites[0]->mSize.y) ? self->mPosition.y + self->mSprites[0]->mSize.y : self->mPosition.y;	
	} else
	{
		position_other = other->mPosition;
	}
	self->mPosition = position_self;
	other->mPosition = position_other;
}