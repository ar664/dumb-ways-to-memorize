#ifndef __GLOBALS_H
#define __GLOBALS_H

#define RETURN_TYPE(X, Y) (X == 1 ? (char*)Y : (X == 4 ? (int*)Y : (int*)Y ) ) /**< Returns pointer to Y based on X size | default (int*) */

#define	FRAME_DELAY		17
#define LEVELS_PER_GAME	6

extern int exitRequest;

extern int CompareMemToMemArray(void *mem, void *mem_array, int size_type, int size_array);

//Prepoccessor defed LINKS and STRICT_MODE
//JSON Parser
#include <jsmn.h>
extern jsmn_parser gParser;
extern jsmntok_t *gTokens;
extern char *gGameData;

#endif