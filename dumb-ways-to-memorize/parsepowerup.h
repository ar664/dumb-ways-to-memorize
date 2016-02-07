#ifndef __PARSE_POWER_UP_H
#define __PARSE_POWER_UP_H


char *InteractionNames[] = {"move", "destroy", "spawn", "edit", "nullify", 0};

void (*ParseToPowerUp(void *Functions[]))(entity_t *self);

//Interactions
void Move();
void Destroy();
void Spawn();
void Edit(void *args, ...);
void Nullify();

//Interactions Array
void (*InteractionSymbols[])() =  {Move, Destroy, Spawn, Edit, Nullify, 0};

#endif