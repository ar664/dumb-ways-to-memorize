#ifndef __PARSE_OBJECT_H
#define __PARSE_OBJECT_H

#include <jsmn.h>

typedef struct object_s object_t;

struct object_s
{
	object_t *parent;
	object_t *children;
	jsmntok_t *keys;
	jsmntok_t *values;
	char *name;
};

object_t *ParseToObject(jsmntok_t *token, char *g_str);
object_t *FindObject(object_t *obj, char *name);
int CountObjectMembers(object_t *obj, char *g_str);
int CountObjectChildren(object_t *obj);
void PrintObject(object_t *obj, char *g_str);
int CopyObjectToObjectArray(object_t **dst, object_t *src, int size);

#endif