#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "globals.h"
#include "mystrings.h"

/**
 * Byte to binary.
 *
 * @param	x	The x coordinate.
 *
 * @return	null if it fails, else a char*.
 *
 * @author	From StackOverFlow
 * @date	2/2/2016
 */

const char *ByteToBinary(int x)
{
    static char b[9];
	int z;
    b[0] = '\0';

    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

/**
 * Searches for the first key and sets token.
 *
 * @param [in,out]	token	The token.
 * @param [in,out]	key  	The key.
 * @param [in,out]	g_str	The string to get from.
 *
 * @return	null if it fails, else the found key.
 *
 * @author	Junji
 * @date	1/30/2016
 */

jsmntok_t *FindKey(jsmntok_t *token, char *key, char *g_str)
{
	char *str;
	jsmntok_t *local_token = token;
	jsmntok_t* endPtr = token + sizeof(token)/sizeof(token[0]);
	while(local_token < endPtr)
	{
		if(token->size == 1)
		{
			str = JsmnToString(local_token, g_str);
			if(!strcmp(str, key))
			{
				return local_token;
			}
			free(str);
		}
		local_token++;
	}
	return NULL;
}

/**
 * Searches for the first value from key.
 *
 * @param [in,out]	token	If non-null, the token.
 * @param [in,out]	key  	If non-null, the key.
 * @param [in,out]	g_str	If non-null, the global string.
 *
 * @return	null if it fails, else the found value from key.
 *
 * @author	Anthony Rios
 * @date	1/30/2016
 */

char * FindValueFromKey(jsmntok_t *token, char *key, char *g_str)
{
	int  i;
	char *str;
	for(i = 0; token[i].type; i++)
	{
		str = JsmnToString(&token[i], g_str);
		if(!strcmp(str,key))
		{
			return JsmnToString(&token[i+1], g_str);
		}
	}
	return NULL;
}

/**
 * Jsmn to string.
 *
 * @param [in,out]	token	If non-null, the token.
 * @param [in,out]	g_str	If non-null, the global string.
 *
 * @return	null if it fails, else a char*.
 *
 * @author	Anthony Rios
 * @date	1/30/2016
 */

char * JsmnToString(jsmntok_t *token, char *g_str)
{
	int size = token->end - token->start;
	char *retVal = ALLOC_STR(size);
	strncpy( retVal, &g_str[token->start], size);
	retVal[size] = 0;
	return retVal;
}

/**
 * Type string from JSON string.
 *
 * @param	Type	The type.
 *
 * @return	null if it fails, else a char*.
 *
 * @author	Anthony Rios
 * @date	1/30/2016
 */

char * TypeFromJSON(jsmntype_t Type)
{
	switch(Type)
	{
	case(JSMN_UNDEFINED):
		return "UNDEFINED";
	case(JSMN_OBJECT):
		return "OBJECT";
	case(JSMN_ARRAY):
		return "ARRAY";
	case(JSMN_STRING):
		return "STRING";
	case(JSMN_PRIMITIVE):
		return "PRIMITIVE";
	default:
		return "UNKOWN";
	}
}

/**
 * File to string.
 *
 * @param [in,out]	fileName	If non-null, filename of the file.
 *
 * @return	null if it fails, else a char*.
 *
 * @author	Anthony Rios
 * @date	1/30/2016
 */

char * FileToString(char *fileName)
{
	char *string;
	int size;
	struct stat st;
	FILE *file;
	stat(fileName, &st);
	file = fopen(fileName, "r");
	if(!file)
	{
		return NULL;
	}
	size = st.st_size;
	string = ALLOC_STR(size);
	size = fread(string, sizeof(unsigned char), size, file);
	string[size] = '\0';
	fclose(file);
	return string;
}