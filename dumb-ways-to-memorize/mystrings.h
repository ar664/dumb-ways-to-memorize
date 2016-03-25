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


char * FileToString(char *file);
char * TypeFromJSON(jsmntype_t Type);
char * JsmnToString(jsmntok_t *token, char *g_str);
char * FindValue(struct object_s * obj, char *key, char *g_str);
void JsmnToInt(jsmntok_t *token, char *str, int *dst);
int StringToInt(char* str);
int CharToInt(char c);
//char * FindValueFromKey(jsmntok_t *token, char *key, char *g_str);
jsmntok_t * FindKey(jsmntok_t *token, char *key, char *g_str);
int ConvertFileToUseable(char *fileName, jsmn_parser *parser, char **stringStorage, jsmntok_t **jsmnStorage);

#endif