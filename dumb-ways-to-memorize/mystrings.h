#ifndef __MYSTRINGS_H
#define __MYSTRINGS_H

#define	ALLOC_STR(X) ( (char *) malloc(sizeof(char)*X) )

#include <jsmn.h>
//int getFileSize(FILE *file);
char * FileToString(char *file);
char * TypeFromJSON(jsmntype_t Type);
char * JsmnToString(jsmntok_t *token, char *str);
void JsmnToInt(jsmntok_t *token, char *str, int *dst);
int StringToInt(char* str);
int CharToInt(char c);
char * FindValueFromKey(jsmntok_t *token, char *key, char *g_str);
jsmntok_t * FindKey(jsmntok_t *token, char *key, char *g_str);


#endif