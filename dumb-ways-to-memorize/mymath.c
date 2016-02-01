#include "globals.h"
#include <string.h>

/**
 * Compare memory to memory array.
 *
 * @param [in,out]	mem		 	If non-null, the memory.
 * @param [in,out]	mem_array	If non-null, array of memories.
 * @param	size_type		 	Size of type.
 * @param	size_array		 	Size of Array.
 *
 * @return	0 if equal, -1 if not.
 *
 * @author	Anthony Rios
 * @date	1/31/2016
 */

int CompareMemToMemArray(void *mem, void *mem_array, int size_type, int size_array)
{
	int i;
	auto memory = RETURN_TYPE(size_type, mem_array);
	for(i = 0; i < size_array; i++, memory++)
	{
		if(!memcmp(mem, memory, size_type))
		{
			memory = NULL;
			return 0;
		}
	}
	memory = NULL;
	return -1;
}
