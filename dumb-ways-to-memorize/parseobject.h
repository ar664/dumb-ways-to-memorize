#ifndef __PARSE_OBJECT_H
#define __PARSE_OBJECT_H

#include <jsmn.h>

typedef struct object_s object_t;

struct object_s
{
	object_t *parent;
	object_t *child;
	jsmntok_t **keys;
	jsmntok_t **data;
};

#endif