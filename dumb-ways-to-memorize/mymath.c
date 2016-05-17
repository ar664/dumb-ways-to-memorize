#include "globals.h"
#include <string.h>
#include <stdlib.h>
#include <chipmunk/chipmunk.h>

int LargestDivisor(int num)
{
	int i, root, hit;
	hit = 0;
	root = num>>1;
	for(i = 0; i < root; i++)
	{
		if(!num%i)
		{
			hit = i;
		}
	}
	return hit;
}

cpVect Vec2Cp(vec2_t* vect)
{
	cpVect cp;
	if(!vect)
	{
		return cpvzero;
	}
	cp.x = (cpFloat) vect->x;
	cp.y = (cpFloat) vect->y;

	return cp;
}

vec2_t CpToVect(cpVect *cp)
{
	vec2_t vect;
	if(!cp)
	{
		return gZeroVect;
	}
	vect.x = (int) cp->x;
	vect.y = (int) cp->y;

	return vect;
}

void Vec2Add(vec2_t *A, vec2_t *B, vec2_t *C)
{
	if(!A || !B || !C)
	{
		return;
	}
	C->x = A->x + B->x;
	C->y = A->y + B->y;
}


void Vec2Subtract(vec2_t *First, vec2_t *Second, vec2_t *C)
{
	if(!First || !Second || !C)
	{
		return;
	}
	C->x = First->x - Second->x;
	C->y = First->y - Second->y;
}

void Vec2MultiplyScalar(vec2_t* A, int B, vec2_t* C)
{
	if(!A || !C)
	{
		return;
	}
	(*C).x = A->x * B;
	(*C).y = A->y * B;
}

int CountMem(void *src, int size_type)
{
	int i, offset;
	int *source = (int*)src;
	if(source == NULL)
		return 0;
	i = 0; offset = size_type/sizeof(int);
	while( *(source) ) 
	{
		source += offset;
		i++;
	}
	return i;
}

int AllocateDynamic(void **dst, void *src, int size_type, int size)
{
	int offset = size_type/sizeof(int);
	if(!dst) return -1;
	//Realloc frees old memory
	*dst = realloc(*dst, size_type*(size+1));
	if(!*dst) return -1;
	src ? memcpy((int*)(*dst)+(size-1)*offset, src, size_type) : memset((int*)(*dst)+(size-1)*offset, 0, size_type);
	memset((int*)(*dst)+(size)*offset, 0, size_type);
	return 0;
}

int CompareMemToMemArray(void *mem, void *mem_array, int size_type, int size_array)
{
	int i, offset;
	int *memory = (int*) mem_array;
	offset = size_type/sizeof(int);
	for(i = 0; i < size_array; i++)
	{
		if(!memcmp(mem, memory, size_type))
		{
			return 0;
		}
		memory += offset;
	}
	return -1;
}
