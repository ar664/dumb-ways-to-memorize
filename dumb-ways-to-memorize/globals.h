#ifndef __GLOBALS_H
#define __GLOBALS_H

#define	FRAME_DELAY		17

extern int exitRequest;

//Prepoccessor defed LINKS and STRICT_MODE
//JSON Parser
#include <jsmn.h>
extern jsmn_parser gParser;
extern jsmntok_t *gTokens;
extern char *gGameData;

#endif