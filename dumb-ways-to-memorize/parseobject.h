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
char *ParseObject();

#endif