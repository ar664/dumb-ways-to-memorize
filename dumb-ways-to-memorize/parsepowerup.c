#include "parsefunction.h"
#include "parsepowerup.h"


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
void Spawn()
{
	//Targ->Spawn
	entity_t *newEnt;
	newEnt = InitNewEntity();
	newEnt->mPosition = targPos;
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
