#include "parseobject.h"
#include "mystrings.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>

/**
 * Parse to object, this object has no parent.
 *
 * @param [in,out]	token	If non-null, the token that begins with object.
 * @param [in,out]	str  	If non-null, the global string.
 *
 * @return	null if it fails, else an object_t*.
 *
 * @author	Anthony Rios
 * @date	2/1/2016
 */

object_t *ParseToObject(jsmntok_t *token, char *g_str)
{
	int objects, keys, values, size, i;
	object_t *currentChild; jsmntok_t *currentKey, *currentValue;
	object_t **children_array; jsmntok_t **keys_array, **values_array;
	object_t *retVal = (object_t*) malloc(sizeof(object_t));
	retVal->name = NULL;
	retVal->parent = NULL;
	size = token->size; i = 0; objects = 0; keys = 0; values = 0;
	currentChild = NULL; currentKey = NULL; currentValue = NULL;
	children_array = NULL; keys_array = NULL; values_array = NULL;
	do
	{
		if(token[i].type == JSMN_OBJECT)
		{
			if(i == 0)
				continue;
			currentChild = ParseToObject(&token[i], g_str);
			objects++;
		} else if(token[i].type == JSMN_STRING)
		{
			if(token[i].size > 0)
			{
				if(token[i+1].type == JSMN_ARRAY || token[i+1].type == JSMN_OBJECT)
				{
					currentChild = ParseToObject(&token[i+1], g_str);
					currentChild->name = JsmnToString(&token[i], g_str);
					objects++;
				} else
				{
					currentKey = &token[i];
					keys++;
				}
			} else
			{
				currentValue = &token[i];
				values++;
			}
		} else
		{
			 currentValue = &token[i];
			 values++;
		}


		if(currentChild)
		{
			if(children_array)
			{
				object_t** temp = children_array;
				children_array = (object_t**) malloc(sizeof(object_t*)*(objects+1));
				memcpy(children_array, temp, sizeof(object_t*)*objects);
			} else
			{
				children_array = (object_t**) malloc(sizeof(object_t*)*(objects+1));
			}
			children_array[objects] = currentChild;
			children_array[objects+1] = NULL;

		} else if(currentKey)
		{
			AllocateDynamic(keys_array, currentKey, SIZE_OF_JSMN_TOK_T, keys);
		} else if (currentValue)
		{
			if(values_array)
			{
				jsmntok_t** temp = values_array;
				values_array = (jsmntok_t**) malloc(sizeof(jsmntok_t*)*(values+1));
				memcpy(values_array, temp, sizeof(jsmntok_t*)*values);
			} else
			{
				values_array = (jsmntok_t**) malloc(sizeof(jsmntok_t*)*(values+1));
			}
			values_array[values] = currentValue;
			values_array[values+1] = NULL;
		} else
		{
			printf("ParseObject Error: idk");
			break;
		}


		i++;
	}while(i < size);
	return retVal;
}