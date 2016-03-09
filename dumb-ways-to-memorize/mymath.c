#include "globals.h"
#include <string.h>
#include <stdlib.h>

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
/**
 * Counts the memory of type size_type, given that the final address is null.
 *
 * @param [in,out]	src	If non-null, source of memory.
 * @param			size_type  	Size of the type.
 *
 * @return	The total number of memory, if src is null 0 is returned.
 *
 * @author	Anthony Rios
 * @date	2/1/2016
 */

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

/**
 * Allocate memory and copy over src into it. Adds Null to end.
 *
 * @param [in,out]	dst	If non-null, destination for the allocation.
 * @param [in,out]	src	If non-null, source for the adding.
 * @param	size_type  	Size of the type.
 * @param	size	   	The size.
 *
 * @return	0 on success, -1 on error.
 *
 * @author	Anthony Rios
 * @date	2/1/2016
 */

int AllocateDynamic(void **dst, void *src, int size_type, int size)
{
	int offset = size_type/sizeof(int);
	if(!dst) return -1;
	*dst = realloc(*dst, size_type*(size+1));
	if(!*dst) return -1;
	src ? memcpy((int*)(*dst)+(size-1)*offset, src, size_type) : memset((int*)(*dst)+(size-1)*offset, 0, size_type);
	memset((int*)(*dst)+(size)*offset, 0, size_type);
	return 0;
}

/**
 * Compare memory to memory array.
 *
 * @param [in,out]	mem		 	If non-null, the memory.
 * @param [in,out]	mem_array	If non-null, array of memories.
 * @param	size_type		 	Size of type, via sizeof() function.
 * @param	size_array		 	Size of Array.
 *
 * @return	0 if equal, -1 if not.
 *
 * @author	Anthony Rios
 * @date	1/31/2016
 */

int CompareMemToMemArray(void *mem, void *mem_array, int size_type, int size_array)
{
	int i, offset;
	int *memory = (int*) mem_array;
	offset = size_type;
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
