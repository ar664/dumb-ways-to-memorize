#ifndef __GLOBALS_H
#define __GLOBALS_H


#include <jsmn.h>
#define RETURN_TYPE(X, Y) (X == sizeof(char) ? (char*)Y  \
						: (X == sizeof(int) ? (int*)Y  \
						: (X == sizeof(jsmntok_t) ? (jsmntok_t*)Y \
						: (int*)Y ) ) ) 

/**< Returns pointer to Y based on X size | default (int*) */
#define STRING_TYPE(X) (X == sizeof(char) ? "Char"  \
						: (X == sizeof(int) ? "Int"  \
						: (X == sizeof(jsmntok_t) ? "Jsmn" \
						: "Unkown" ) ) ) 
/**< Returns string of the type of size X*/

typedef struct vec2_s
{
	int x;
	int y;
}vec2_t;

#define HAZARD_NULL	0x0
#define HAZARD_MAX	(0x1 >> 31)
extern char **Hazards_str;
extern int StrToHazard(char *str);

//For PowerUps
extern vec2_t mousePos;
extern int keyPower;

extern void *Player;
extern void *World;
extern void *Mouse;

#define	FRAME_DELAY		17
#define LEVELS_PER_GAME	6

extern int exitRequest;

extern int CountMem(void *src, int size_type);
extern int AllocateDynamic(void **dst, void *src, int size_type, int size);
extern int CompareMemToMemArray(void *mem, void *mem_array, int size_type, int size_array);

//Prepoccessor defed LINKS and STRICT_MODE
//JSON Parser
extern jsmn_parser gParser;
extern jsmntok_t *gTokens;
extern char *gGameData;

#endif