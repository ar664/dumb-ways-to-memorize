#ifndef __ENTITY__H
#define __ENTITY__H

typedef struct entity_s entity_t;

struct entity_s
{
	int mCollisionType;
	int mStatus;
	void (*Think)(entity_t *self);

};


#endif