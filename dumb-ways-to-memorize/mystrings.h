#ifndef __MYSTRINGS_H
#define __MYSTRINGS_H

#define	ALLOC_STR(X) ( (char *) malloc(sizeof(char)*X) )

#include <jsmn.h>
struct object_s;

#define GAME_STATE_SPLASH_STR	"Splash"
#define GAME_STATE_START_STR	"Start"
#define GAME_STATE_GUESS_STR	"Guess"
#define GAME_STATE_CHOOSE_STR	"Choose"
#define GAME_STATE_PLAYING_STR	"Playing"
#define GAME_STATE_END_STR		"End"

/**
 * Loads string data from a file.
 *
 * @param [in,out]	fileName	If non-null, filename of the file.
 *
 * @return	null if it fails, else a char*.
 *
 * @author	Anthony Rios
 * @date	1/30/2016
 */
char * FileToString(char *file);

/**
 * Type string from JSON string.
 *
 * @param	Type	The type.
 *
 * @return	null if it fails, else a char*.
 *
 * @author	Anthony Rios
 * @date	1/30/2016
 */
char * TypeFromJSON(jsmntype_t Type);

/**
 * Jsmn to string.
 *
 * @param [in,out]	token	If non-null, the token.
 * @param [in,out]	g_str	If non-null, the global string.
 *
 * @return	null if it fails, else a char*.
 *
 * @author	Anthony Rios
 * @date	1/30/2016
 */
char * JsmnToString(jsmntok_t *token, char *g_str);

/**
 * Searches for the a value in the object that matches the key string given.
 * Recursive.
 *
 * @param [in,out]	obj  	If non-null, the object.
 * @param [in,out]	key  	If non-null, the key.
 * @param [in,out]	g_str	If non-null, the string.
 *
 * @return	null if it fails, else the found value.
 *
 * @author	Anthony Rios
 * @date	3/25/2016
 */
char * FindValue(struct object_s * obj, char *key, char *g_str);

/**
 * Jsmn to int.
 *
 * @param [in,out]	token	If non-null, the token.
 * @param [in,out]	str  	If non-null, the string.
 * @param [in,out]	dst  	If non-null, destination for the value.
 *
 * @author	Anthony Rios
 * @date	2/28/2016
 */
void JsmnToInt(jsmntok_t *token, char *str, int *dst);

/**
 * String to int.
 *
 * @param [in,out]	str	If non-null, the string.
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	2/28/2016
 */
int StrToInt(char* str);

/**
 * Character to int. A cheap switch statement.
 *
 * @param	c	The character.
 *
 * @return	An int.
 *
 * @author	Anthony Rios
 * @date	2/28/2016
 */
int CharToInt(char c);

/**
 * Searches for the first key that matches given gPlayerName, through through tokens & g_str.
 *
 * @param [in,out]	token	The token.
 * @param [in,out]	key  	The key.
 * @param [in,out]	g_str	The string to get from.
 *
 * @return	null if it fails, else the found key.
 *
 * @author	Junji
 * @date	1/30/2016
 */
jsmntok_t * FindKey(jsmntok_t *token, char *key, char *g_str);


/**
 * Convert file to useable, by setting the storage for string and jsmntokens, give a filename.
 *
 * @param [in,out]	fileName	 	If non-null, filename of the file.
 * @param [in,out]	parser		 	If non-null, the parser.
 * @param [in,out]	stringStorage	If non-null, the string storage.
 * @param [in,out]	jsmnStorage  	If non-null, the jsmn storage.
 *
 * @return	The file converted to useable.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */
int ConvertFileToUseable(char *fileName, jsmn_parser *parser, char **stringStorage, jsmntok_t **jsmnStorage);

#endif