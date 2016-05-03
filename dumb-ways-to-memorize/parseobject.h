#ifndef __PARSE_OBJECT_H
#define __PARSE_OBJECT_H

#include <jsmn.h>
#ifndef _INC_STDIO
	#include <stdio.h>
#endif
typedef struct object_s object_t;
typedef struct string_object_s string_object_t;
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
	object_t *parent;   /**< Pointer to Parent Object*/
	object_t *children; /**< Array of children objects*/
	jsmntok_t *keys;	/**< Array of keys */
	jsmntok_t *values;  /**< Array of values related to those keys */
	char *name;			/**< The name of the object */
};

/**
 * A file object structure used for temporary file creation.
 * These should be freed after used
 *
 * @author	Anthony Rios
 * @date	4/25/2016
 */

struct string_object_s
{
	string_object_s *parent;
	string_object_s *children;
	char **keys;
	char **values;
	char *name;
};

/**
 * Parse to object, this object has no parent.
 *
 * @param [in,out]	token	If non-null, the token that begins with object.
 * @param [in,out]	g_str  	If non-null, the global string data of given tokens.
 *
 * @return	null if it fails, else an object_t*.
 *
 * @author	Anthony Rios
 * @date	2/1/2016
 */
object_t *ParseToObject(jsmntok_t *token, char *g_str);

/**
 * Searches for the first object in object list that matches name.
 *
 * @param [in,out]	obj 	If non-null, the object.
 * @param [in,out]	name	If non-null, the name.
 *
 * @return	null if it fails, else the found object.
 *
 * @author	Anthony Rios
 * @date	2/27/2016
 */
object_t *FindObject(object_t *obj, char *name);

//Recursive Member Count
int CountObjectMembers(object_t *obj, char *g_str);

//Recursive Children Count
int CountObjectChildren(object_t *obj);

// Recursive Print for Objects
void PrintObject(object_t *obj, char *g_str);

// Copy's object into a object_array ,dynamically allocates if dst not big enough
int CopyObjectToObjectArray(object_t **dst, object_t *src, int size);

void AddObject2StrObj(string_object_t *obj, string_object_t *data);
void AddValue2StrObj(string_object_t *obj, char *data);
void AddKVPair2StrObj(string_object_t *obj, char *key, char *data);

void PrintStringObject(string_object_t* obj);
void WriteStringObjectToFile(string_object_t *obj, FILE* file, int depth);

extern string_object_t *editor_object;

#endif