#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include "globals.h"
#include "mystrings.h"



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
	jsmntok_t* endPtr = token + CountMem(token, sizeof(jsmntok_t));
	if(!local_token || !g_str || !key)
		return local_token;
	while(local_token < endPtr)
	{
		if(token->size == 1)
		{
			str = JsmnToString(local_token, g_str);
			if(!strcmp(str, key))
			{
				if(str) free(str);
				return local_token;
			}
			if(str) free(str);
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
	char *retVal = ALLOC_STR(size+1);
	if(!retVal)
	{
		printf("Could not allocate memory");
		exit(-1);
	}
	strncpy( retVal, &g_str[token->start], size);
	retVal[size] = 0;
	return retVal;
}

void JsmnToInt(jsmntok_t* token, char* str, int* dst)
{
	char *temp;
	temp = JsmnToString(token, str);
	*dst = StringToInt(temp);
	if(temp) free(temp);
}

int StringToInt(char* str)
{
	int i, retVal, length;
	if(str == NULL)
	{
		return 0;
	}
	length = strlen(str);
	retVal = 0;
	for(i = 0; i < length; i++)
	{
		retVal += CharToInt(str[i])*(10^(length-i-1));
	}
	return retVal;
}

int CharToInt(char c)
{
	switch(c)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default:
		return 0;
	}
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
	if(!string)
	{
		printf("Could not allocate memory");
		exit(-1);
	}
	size = fread(string, sizeof(unsigned char), size, file);
	string[size] = '\0';
	fclose(file);
	return string;
}