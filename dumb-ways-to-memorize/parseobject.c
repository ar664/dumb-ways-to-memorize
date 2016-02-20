#include "parseobject.h"
#include "mystrings.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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
	object_t *children_array; jsmntok_t *keys_array, *values_array;
	object_t *retVal = (object_t*) malloc(sizeof(object_t));
	retVal->name = NULL;
	retVal->parent = NULL;
	size = token->end; i = 0; objects = 0; keys = 0; values = 0;
	currentChild = NULL; currentKey = NULL; currentValue = NULL;
	children_array = NULL; keys_array = NULL; values_array = NULL;
	do
	{
		if(token[i].type == JSMN_OBJECT)
		{
			if(i == 0)
			{
				i++;
				continue;
			}
			currentChild = ParseToObject(&token[i], g_str);
			currentChild->parent = retVal;
			objects++;
			i += CountMem(currentChild->keys, sizeof(jsmntok_t*)) +
				CountMem(currentChild->values,sizeof(jsmntok_t*)) + 
				CountMem(currentChild->children,sizeof(jsmntok_t*));
			continue;
		} else if(token[i].type == JSMN_STRING)
		{
			if(token[i].size > 0)
			{
				if(token[i+1].type == JSMN_ARRAY || token[i+1].type == JSMN_OBJECT)
				{
					currentChild = ParseToObject(&token[i+1], g_str);
					currentChild->name = JsmnToString(&token[i], g_str);
					currentChild->parent = retVal;
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
		} else if(token[i].type == JSMN_ARRAY)
		{
			if(i == 0)
				i++;
				continue;
		} else
		{
			 currentValue = &token[i];
			 values++;
		}

		//Doing & to pass reference instead of value
		if(currentChild)
		{
			AllocateDynamic(&children_array, currentChild, sizeof(object_t), objects);
			i += CountMem(currentChild->keys, sizeof(jsmntok_t*)) +
				CountMem(currentChild->values,sizeof(jsmntok_t*)) + 
				CountMem(currentChild->children,sizeof(jsmntok_t*));
			currentChild = NULL;
		} else if(currentKey)
		{
			AllocateDynamic(&keys_array, currentKey, sizeof(jsmntok_t), keys);
			currentKey = NULL;
		} else if (currentValue)
		{
			AllocateDynamic(&values_array, currentValue, sizeof(jsmntok_t), values);
			currentValue = NULL;
		} else
		{
			printf("ParseObject Error: idk");
			break;
		}
		i++;
	}while( (size > token[i].start) && token[i].type);
	retVal->values = values_array;
	retVal->keys = keys_array;
	retVal->children = children_array;
	return retVal;
	}

object_t *FindObject(object_t *obj, char *name)
{
	int i;
	object_t *retVal, *temp;
	retVal = NULL;
	if(obj == NULL || name == NULL)
		return retVal;
	temp = obj;
	for(i = 0; temp[i].name != NULL; i++)
	{
		if(!temp[i].name)
			continue;
		if(!strcmp(temp[i].name, name) )
		{
			return &temp[i];
		}
	}
	return retVal;
}