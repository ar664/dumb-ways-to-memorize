#include <stdio.h>
#include <stdlib.h>
#include "mystrings.h"



char * FileToString(FILE *file)
{
	int size;
	char *string;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	string = (char *) malloc(sizeof(char)*size);
	if(!string) perror("File to string went wrong");
	return string;
}