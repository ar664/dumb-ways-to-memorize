#ifndef __PARSE_OBJECT_H
#define __PARSE_OBJECT_H

#include <jsmn.h>

typedef struct object_s object_t;

/**
 * An object structure that is used for a better parsing system taken from jsmn tokens.
 *
 * key + value = single key address + single value address
 * object = single key + multiple values
 * 
 * @author	Anthony Rios
 * @date	3/30/2016
 */

struct object_s
{
	object_t *parent;
	object_t *children;
	jsmntok_t *keys;	/**< Array of keys */
	jsmntok_t *values;  /**< Array of values related to those keys */
	char *name;			/**< The name of the object */
};

object_t *ParseToObject(jsmntok_t *token, char *g_str);
object_t *FindObject(object_t *obj, char *name);
int CountObjectMembers(object_t *obj, char *g_str);
int CountObjectChildren(object_t *obj);
void PrintObject(object_t *obj, char *g_str);
int CopyObjectToObjectArray(object_t **dst, object_t *src, int size);

#endif