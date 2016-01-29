#include <stdlib.h>
#include <string.h>
#include "mystrings.h"
#include <sys/types.h>
#include <sys/stat.h>	

char * JsmnToString(jsmntok_t *token, char *str)
{
	int i;
	int size = token->end - token->start;
	char *retVal = ALLOC_STR(size);
	for(i = 0; i < size; i++)
	{
		memcpy( (void*)retVal[i],(void*)str[i+token->start], sizeof(char));
	}
	retVal[size] = 0;
	return retVal;
}

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