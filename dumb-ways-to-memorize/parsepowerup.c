#include "parsefunction.h"
#include "parsepowerup.h"
#include "parseobject.h"
#include "entity.h"
#include "mystrings.h"
#include <jsmn.h>


//Needs math and time
void Move()
{
	//Targ->Move to Pos
	targEnt->mVelocity = targPos; //TODO math
}


//No other access necessary
void Destroy()
{
	//freeEnt(Targ)
}

//Needs access to parseEntity
void Spawn(object_t *newObj, char *g_str)
{
	//Targ->Spawn
	int i;
	entity_t *newEnt;
	jsmntok_t *tempTok;
	newEnt = InitNewEntity();
	tempTok = newObj->keys;
	for(i = 0; tempTok->type; i++, tempTok++ )
	{

	}
}

//Needs more information
void Edit(void *args, ...)
{
	//Targ->Edit
}

//No other access neccesary
void Nullify()
{
	//Targ->Think = NULL;
	targEnt->Think = NULL;
}
