#include <stdlib.h>
#include <string.h>
#include "mystrings.h"
#include <sys/types.h>
#include <sys/stat.h>	

//Old verision using fseek/fset
/*
int getFileSize(FILE *file)
{
	int size;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

char * FileToString(FILE *file)
{
	int size, i;
	char *string, *retString;
	size = 1;
	string = (char *) malloc(sizeof(char)*size);
	retString = string;
	fscanf(file, "%[^\0]", string);
	while(!feof(file))
	{
		memset(string, fgetc(file), sizeof(char));
		string++;
	}
	retString[size] = '\0';
	if(!retString) perror("File to string went wrong");
	return retString;
}
*/

//New Version using sys/stats & fgets

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