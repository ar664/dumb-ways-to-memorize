#ifndef __MYSTRINGS_H
#define __MYSTRINGS_H

#define	ALLOC_STR(X) ( (char *) malloc(sizeof(char)*X) )

#include <jsmn.h>
#include <stdio.h>
//int getFileSize(FILE *file);
const char *ByteToBinary(int x);
char * FileToString(char *file);
char * TypeFromJSON(jsmntype_t Type);
char * JsmnToString(jsmntok_t *token, char *str);
char * FindValueFromKey(jsmntok_t *token, char *key, char *g_str);
char * FindKey(jsmntok_t *token, char *key, char *g_str);


#endif