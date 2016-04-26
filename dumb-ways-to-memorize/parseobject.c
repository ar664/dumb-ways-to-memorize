#include "parseobject.h"
#include "mystrings.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

object_t *ParseToObject(jsmntok_t *token, char *g_str)
{
	int objects, keys, values, size, i;
	char *tempStr;
	object_t *currentChild; jsmntok_t *currentKey, *currentValue;
	object_t *children_array; jsmntok_t *keys_array, *values_array;
	object_t *retVal = (object_t*) malloc(sizeof(object_t)*2);

	//Err Check
	if(!g_str || !token)
	{
		printf("Parse Obj given NULL token or Str");
		return NULL;
	}

	//Init Vars
	retVal->name = NULL;
	retVal->parent = NULL;
	size = token->end; i = 0; objects = 0; keys = 0; values = 0;
	currentChild = NULL; currentKey = NULL; currentValue = NULL;
	children_array = NULL; keys_array = NULL; values_array = NULL;

	//Parse
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
		} else if(token[i].type == JSMN_STRING)
		{
			if(token[i].size > 0)
			{
				if(token[i+1].type == JSMN_ARRAY || token[i+1].type == JSMN_OBJECT)
				{
					currentChild = ParseToObject(&token[i+1], g_str);
					currentChild->name = JsmnToString(&token[i], g_str);
					currentChild->parent = retVal;
					i += token[i].size;
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
			{
				i++;
				continue;
			}
			currentChild = ParseToObject(&token[i], g_str);
			currentChild->parent = retVal;
			objects++;
		} else
		{
			 currentValue = &token[i];
			 values++;
		}

		//Doing & to pass reference instead of value
		if(currentChild)
		{
			if ( CopyObjectToObjectArray(&children_array, currentChild, objects) == -1 )
			{
				printf("Dynam Alloc error : parseobj \n");
			}
			//tempStr = JsmnToString(&token[i], g_str);
			//printf("Token %d Size : %d CountObjectMems : %d \n %s \n", i, token[i].size, CountObjectMembers(currentChild, g_str), tempStr);
			i += CountObjectMembers(currentChild, g_str);
			currentChild = NULL;
		} else if(currentKey)
		{
			if( AllocateDynamic((void**) &keys_array, currentKey, sizeof(jsmntok_t), keys) == -1 )
			{
				printf("Dynam Alloc eror : parseobj \n");
			}
			currentKey = NULL;
		} else if (currentValue)
		{
			if (AllocateDynamic( (void**) &values_array, currentValue, sizeof(jsmntok_t), values) == -1 )
			{
				printf("Dynam Alloc eror : parseobj \n");
			}
			currentValue = NULL;
		} else
		{
			printf("ParseObject Error: idk");
			break;
		}
		i++;
	}while( (size > token[i].start) && (token[i].type > 0) );
	retVal->values = values_array;
	retVal->keys = keys_array;
	retVal->children = children_array;
	memset(&retVal[1], 0, sizeof(object_t));
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
	if(temp->children)
	{
		return FindObject(temp->children, name);
	}
	return retVal;
}

int CountObjectMembers(object_t* obj, char* g_str)
{
	int objects, objCount, retVal, i;
	if(!obj || !g_str)
	{
		return 0;
	}
	objects = 1 + CountMem(obj->children, sizeof(object_t));
	objCount = 0;
	retVal = 0;
	while(objCount < objects)
	{
		if(objCount == 0)
		{
			retVal += CountMem(obj->keys, sizeof(jsmntok_t));
			retVal += CountMem(obj->values, sizeof(jsmntok_t));
		} else
		{
			retVal += CountObjectMembers(&obj->children[objCount-1], g_str) + 1;
		}
		objCount++;
	}

	return retVal;
}

int CountObjectChildren(object_t *obj)
{
	int objects, temp, children;
	objects = CountMem(obj->children, sizeof(object_t));
	temp = 0;
	children = 0;
	while (temp < objects)
	{
		if(obj->children[temp].children)
		{
			children += CountObjectChildren(obj->children);
		}
		children++;
		temp++;
	}
	return children;
}

void PrintObject(object_t* obj, char *g_str)
{
	int objects, tempInt, i;
	if(!obj || !g_str)
	{
		return;
	}
	objects = 1 + CountMem(obj->children, sizeof(object_t));
	tempInt = 0;
	while(tempInt < objects)
	{
		if(tempInt == 0)
		{
			if(obj->name) 
			{
				printf("%s ", obj->name);
			}
			printf("{ \n");
			if(obj->keys && obj->values)
			{
				i = 0;
				while(obj->keys[i].type && obj->values[i].type)
				{
					printf("Key : %s \n", JsmnToString(&obj->keys[i], g_str));
					printf("Value : %s \n", JsmnToString(&obj->values[i], g_str));
					i++;
				}
			} else if(obj->values)
			{
				i = 0;
				while(obj->values[i].type)
				{
					printf("Value : %s \n", JsmnToString(&obj->values[i], g_str));
					i++;
				}
			}
			tempInt++;
			continue;
		}
		if(&obj->children[tempInt-1] < &obj->children[objects])
		{
			PrintObject(&obj->children[tempInt-1], g_str);
		}
		tempInt++;
	}
	printf("} \n\n");

}

int CopyObjectToObjectArray(object_t **dst, object_t *src, int size)
{
	int count, i;
	if(!dst || !src)
	{
		return -1;
	}
	*dst = (object_t*) realloc(*dst, sizeof(object_t)*(size+1));
	if(!*dst)
	{
		return -1;
	}
	if(size == 1)
	{
		memset(*dst, 0, sizeof(object_t));
	}
	((*dst)[size-1]).name = src->name;
	((*dst)[size-1]).parent = src->parent;
	if(src->keys)
	{
		count = CountMem(src->keys, sizeof(jsmntok_t));
		for(i = 0; i < count+1; i++)
		{
			if( AllocateDynamic( (void**) &((*dst)[size-1]).keys, &src->keys[i], sizeof(jsmntok_t), i+1)  == -1)
			{
				return -1;
			}
		}
		
	}
	if(src->values)
	{
		count = CountMem(src->values, sizeof(jsmntok_t));
		for(i = 0; i < count+1; i++)
		{
			if( AllocateDynamic( (void**) &((*dst)[size-1]).values, &src->values[i], sizeof(jsmntok_t), i+1)  == -1)
			{
				return -1;
			}
		}
	}
	if(src->children)
	{
		count = CountMem(src->children, sizeof(object_t ));
		for(i = 0; i < count+1; i++)
		{
			if( CopyObjectToObjectArray( &((*dst)[size-1]).children, &src->children[i], i+1)  == -1)
			{
				return -1;
			}
		}
	}
	memset(&(*dst)[size], 0, sizeof(object_t));
	return 0;
}

void AddObject2StrObj(string_object_t *obj, string_object_t *data)
{
	int objects;
	if(!obj || !data)
	{
		return;
	}
	objects = CountMem(obj->children, sizeof(string_object_t));
	AllocateDynamic((void**)&obj->children, (void*)data, sizeof(string_object_t), objects);
}

void AddValue2StrObj(string_object_t *obj, char *data)
{
	int items;
	if(!obj || !data)
	{
		return;
	}
	items = CountMem(obj->values, sizeof(char*));
	AllocateDynamic((void**)&obj->values, (void*)&data, sizeof(char*), items);
}

void AddKVPair2StrObj(string_object_t *obj, char *key, char *data)
{
	int check;
	if(!obj || !key || !data)
	{
		return;
	}
	check = CountMem(obj->values, sizeof(char*)) + CountMem(obj->keys, sizeof(char*));
	if( !(check%2) )
	{
		AllocateDynamic((void**)&obj->keys, (void*)&key, sizeof(char*), check/2);
		AllocateDynamic((void**)&obj->values, (void*)&data, sizeof(char*), check/2);
	}

}

void PrintStringObject(string_object_t* obj)
{
	int objects, tempInt, i;
	if(!obj)
	{
		return;
	}
	objects = 1 + CountMem(obj->children, sizeof(string_object_t));
	tempInt = 0;
	while(tempInt < objects)
	{
		if(tempInt == 0)
		{
			if(obj->name) 
			{
				printf("%s ", obj->name);
			}
			printf("{ \n");
			if(obj->keys && obj->values)
			{
				i = 0;
				while(obj->keys[i] && obj->values[i])
				{
					printf("Key : %s \n", obj->keys[i]);
					printf("Value : %s \n", obj->values[i]);
					i++;
				}
			} else if(obj->values)
			{
				i = 0;
				while(obj->values[i])
				{
					printf("Value : %s \n", obj->values[i]);
					i++;
				}
			}
			tempInt++;
			continue;
		}
		if(&obj->children[tempInt-1] < &obj->children[objects])
		{
			PrintStringObject(&obj->children[tempInt-1]);
		}
		tempInt++;
	}
	printf("} \n\n");

}

void WriteMembersToFile(FILE *file, char *key, char *value, bool comma)
{
	if(key)
	{
		fprintf(file, "\t%s : ", key);
	}
	if(value)
	{
		fprintf(file, "%s", value);
	}
	if(comma)
	{
		fprintf(file, ", \n");
	} else
	{
		fprintf(file, "\n");
	}
}

void WriteStringObjectToFile(string_object_t *obj, FILE* file, int depth)
{
	int i, j, k, object_count, key_count, value_count, pairs, value_only_count;
	bool comma;
	char depth_tab[32];
	if(!obj)
	{
		printf("No obj given to write string object \n");
		return;
	}
	if(!file)
	{
		printf("Unable to open file for string object writing \n");
		return;
	}
	object_count = 1+ CountMem(obj->children, sizeof(string_object_t));
	for(i = 0; i < depth; i++)
	{
		depth_tab[i] = '/t';
	}
	depth_tab[i] = NULL;
	for(i = 0; i < object_count; i++)
	{
		if(i == 0)
		{
			if(depth_tab[0] != NULL)
			{
				fprintf(file, depth_tab);
			}
			fprintf(file, "{ \n");
			key_count = CountMem(obj->keys, sizeof(char*));
			value_count = CountMem(obj->values, sizeof(char*));
			pairs = key_count - (value_count-key_count);
			value_only_count = value_count - 2*pairs;
			
			//Write Pairs before values
			for(j = 0; j < pairs; j++)
			{
				if(depth_tab[0] != NULL)
				{
					fprintf(file, depth_tab);
				}
				comma = value_only_count ? 1 : pairs-j-1;
				WriteMembersToFile(file, obj->keys[j], obj->values[j], comma);
			}

			//Write values
			for(j = 0; j < value_only_count; j++)
			{
				WriteMembersToFile(file, NULL, obj->values[j+pairs], pairs-j-1);
			}
		} else
		{
			WriteStringObjectToFile(&obj->children[i-1], file, depth+1);
		}
	}
	if(depth_tab[0] != NULL)
	{
		fprintf(file, depth_tab);
	}
	fprintf(file, "} \n");
}