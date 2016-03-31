#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include "globals.h"
#include "mystrings.h"
#include "parseobject.h"

jsmntok_t *FindKey(jsmntok_t *token, char *key, char *g_str)
{
	char *str;
	jsmntok_t *local_token = token;
	jsmntok_t* endPtr = token + CountMem(token, sizeof(jsmntok_t));
	if(!local_token || !g_str || !key)
		return local_token;
	while(local_token < endPtr)
	{
		if(local_token->size == 1)
		{
			str = JsmnToString(local_token, g_str);
			if(!strcmp(str, key))
			{
				if(str) free(str);
				return local_token;
			}
			if(str) free(str);
		}
		local_token++;
	}
	return NULL;
}

char * JsmnToString(jsmntok_t *token, char *g_str)
{
	int size = token->end - token->start;
	char *retVal;
	if(size < 0)
	{
		return NULL;
	}
	retVal = ALLOC_STR(size+1);
	if(!retVal)
	{
		printf("Could not allocate memory");
		exit(-1);
	}
	strncpy( retVal, &g_str[token->start], size);
	retVal[size] = 0;
	return retVal;
}

char* FindValue(struct object_s* obj, char* key, char* g_str)
{
	int i, keys, objects;
	char *temp_str;
	keys = CountMem(obj->keys, sizeof(jsmntok_t));
	objects = CountMem(obj->children, sizeof(object_t));
	//Iterate through keys
	for(i = 0; i < keys; i++)
	{
		temp_str = JsmnToString(&obj->keys[i], g_str);
		if(!temp_str)
			continue;
		if(!strcmp(temp_str, key))
		{
			if(temp_str) free(temp_str);
			return JsmnToString(&obj->values[i], g_str);
		}
		if(temp_str) free(temp_str);
	}
	//Iterate through children
	for(i = 0; i < objects; i++)
	{
		if( (temp_str = FindValue(&obj->children[i], key, g_str)) != NULL)
		{
			return temp_str;
		}
		if(temp_str) free(temp_str);
	}
	return NULL;
}

void JsmnToInt(jsmntok_t* token, char* str, int* dst)
{
	char *temp;
	temp = JsmnToString(token, str);
	*dst = StrToInt(temp);
	if(temp) free(temp);
}

int StrToInt(char* str)
{
	int i, retVal, length, neg;
	if(str == NULL)
	{
		return 0;
	}
	length = strlen(str);
	retVal = 0;
	neg = 0;
	if(str[0] == '-')
	{
		i = 1;
		neg = 1;
	} else
	{
		i = 0;
	}
	for(i; i < length; i++)
	{
		retVal += CharToInt(str[i])*(powf( 10, (length-i-1) ) );
	}
	if(neg)
	{
		retVal *= -1;
	}
	return retVal;
}

int CharToInt(char c)
{
	switch(c)
	{
	case '0': return 0;
	case '1': return 1;
	case '2': return 2;
	case '3': return 3;
	case '4': return 4;
	case '5': return 5;
	case '6': return 6;
	case '7': return 7;
	case '8': return 8;
	case '9': return 9;
	default:
		return 0;
	}
}

char * TypeFromJSON(jsmntype_t Type)
{
	switch(Type)
	{
	case(JSMN_UNDEFINED):
		return "UNDEFINED";
	case(JSMN_OBJECT):
		return "OBJECT";
	case(JSMN_ARRAY):
		return "ARRAY";
	case(JSMN_STRING):
		return "STRING";
	case(JSMN_PRIMITIVE):
		return "PRIMITIVE";
	default:
		return "UNKOWN";
	}
}

char * FileToString(char *fileName)
{
	char *string;
	int size;
	struct stat st;
	FILE *file;
	stat(fileName, &st);
	file = fopen(fileName, "r");
	if(!file)
	{
		return NULL;
	}
	size = st.st_size;
	string = ALLOC_STR(size);
	if(!string)
	{
		printf("Could not allocate memory");
		exit(-1);
	}
	size = fread(string, sizeof(unsigned char), size, file);
	string[size] = '\0';
	fclose(file);
	return string;
}


enum FreeVar
{
	PARSER=	0x1,
	STRING=	0x2,
	TOKEN=	0x4
};

int ConvertFileToUseable(char *fileName, jsmn_parser *parser, char **stringStorage, jsmntok_t **jsmnStorage)
{
	int num_tokens, varsToFree = 0;
	jsmn_parser *tempPars;
	char **tempStr;
	jsmntok_t **tempJsmn;
	if(!fileName)
	{
		printf("Convert File given NULL filename");
		return -1;
	}

	//Init parser, if not given
	if(!parser)
	{
		tempPars = (jsmn_parser*) malloc(sizeof(parser));
		if(!tempPars) return -1;
		parser = tempPars;
		varsToFree |= PARSER;
	}
	jsmn_init(parser);

	//Init stringStorage if not given
	if(!stringStorage)
	{
		tempStr = (char**) malloc(sizeof(char*));
		if(!tempStr) return -1;
		stringStorage = tempStr;
		varsToFree |= STRING;
	}
	*stringStorage = FileToString(fileName);
	if(!*stringStorage)
	{
		return -1;
	}

	//Init jsmnStorage if not given
	if(!jsmnStorage)
	{
		tempJsmn = (jsmntok_t**) malloc(sizeof(jsmntok_t*));
		if(!tempJsmn) return -1;
		jsmnStorage = tempJsmn;
		varsToFree |= TOKEN;
	}

	//Actual Parsing
	num_tokens = jsmn_parse(parser, *stringStorage, strlen(*stringStorage), NULL, 0);
	if(num_tokens < 1)
	{
		printf("Jsmn Parse Eror: %d", num_tokens);
		return -1;
	}
	*jsmnStorage = (jsmntok_t*) malloc(sizeof(jsmntok_t)*(num_tokens+1));
	if(!*jsmnStorage)
	{
		return -1;
	}
	jsmn_init(parser); //Reset parser
	num_tokens = jsmn_parse(parser, *stringStorage, strlen(*stringStorage), *jsmnStorage, num_tokens);
	printf("Jsmn returned : %d\n", num_tokens);
	
	memset( &(*jsmnStorage)[num_tokens], 0, sizeof(jsmntok_t));

	//Freeing if necessary
	if(varsToFree & PARSER) ;
	if(varsToFree & STRING) free(*stringStorage);
	if(varsToFree & TOKEN) free(*jsmnStorage);

	return num_tokens;
}

/**
 * Converts a str to a game state.
 *
 * @param [in,out]	str	If non-null, the string.
 *
 * @return	str as a GameState.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

GameState StrToGameState(char *str)
{
	if(!str)
	{
		return SPLASH;
	}
	else if(!strcmp(str, GAME_STATE_SPLASH_STR))
	{
		return SPLASH;
	}
	else if(!strcmp(str, GAME_STATE_START_STR))
	{
		return START;
	}
	else if(!strcmp(str, GAME_STATE_GUESS_STR))
	{
		return GUESS;
	} else if(!strcmp(str, GAME_STATE_CHOOSE_STR))
	{
		return CHOOSE;
	}
	else if(!strcmp(str, GAME_STATE_PLAYING_STR))
	{
		return PLAYING;
	}
	if(!strcmp(str, GAME_STATE_END_STR))
	{
		return END;
	}
	return SPLASH;
}

/**
 * Converts a str to a hazard.
 *
 * @param [in,out]	str	If non-null, the string.
 *
 * @return	str as an int.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

int StrToHazard(char *str)
{
	int i, length;
	if(!str)
		return 0;
	length = CountMem(Hazards_str, sizeof(char*));
	for(i = 0; i < length; i++)
	{
		if(!strcmp(str, Hazards_str[i]))
		{
			return (1 << i);
		}
	}
	return 0;
}

/**
 * Converts a str to a collision type.
 *
 * @param [in,out]	str	If non-null, the string.
 *
 * @return	str as a collision_type_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

collision_type_t StrToCollisionType(char *str)
{
	int i;
	if(!str)
		return 0;
	for(i = 0; i <= COLLISION_TYPE_CLIP; i++)
	{
		if(!strcmp(Collisions_str[i], str))
		{
			return (collision_type_t)i;
		}
	}
	return 0;
}

/**
 * Converts a str to an entity state.
 *
 * @param [in,out]	str	If non-null, the string.
 *
 * @return	str as an entity_state_t.
 *
 * @author	Anthony Rios
 * @date	3/29/2016
 */

entity_state_t StrToEntityState(char *str)
{
	int i;
	if(!str)
		return 0;
	for(i = 0; i <= COLLISION_TYPE_CLIP; i++)
	{
		if(!strcmp(EntityStates_str[i], str))
		{
			return (entity_state_t)i;
		}
	}
	return 0;
}

vec2_t* ParseToVec2(object_t* object, char* str)
{
	vec2_t *retVal;
	char *temp1, *temp2;
	retVal = (vec2_t*) malloc(sizeof(vec2_t));
	if(!retVal || !object || !str)
	{
		return NULL;
	}
	temp1 = JsmnToString(&object->values[0], str);
	temp2 = JsmnToString(&object->values[1], str);
	retVal->x = StrToInt(temp1);
	retVal->y = StrToInt(temp2);
	free(temp1); free(temp2);
	return retVal;
}

char **ParseToStringArray(object_t* object, char* str)
{
	int i, size;
	char *temp;
	char **retVal;
	if(!object || !str)
		return NULL;
	size = CountMem(object->values, sizeof(jsmntok_t));
	retVal = (char**) malloc(sizeof(char*)*(size+1));
	for(i = 0; i < size; i++)
	{
		temp = JsmnToString(&object->values[i], str);
		retVal[i] = temp;
	}
	retVal[size+1] = 0;
	return retVal;
}