#ifndef __AUDIO__H
#define __AUDIO__H

#include "SDL_mixer.h"
#define MAX_SOUNDS	100

/** Defines an enum for sound mixer groups */
typedef enum
{
	SOUND_GROUP_PLAYER,
	SOUND_GROUP_OTHER_ENTS,
	SOUND_GROUP_MUSIC,
	SOUND_GROUP_OTHER = -1
}sound_mixer_group;

/** Defines an alias representing the enum for different sound effects */
typedef enum
{
	SOUND_EFFECT_NULL,
	SOUND_EFFECT_WALK,
	SOUND_EFFECT_ATTACK,
	SOUND_EFFECT_JUMP,
	SOUND_EFFECT_DIE,
}sound_mixer_effects;

typedef struct sound_s sound_t;

//Unfinished structure of the entities
struct sound_s
{
	sound_mixer_group group_id;
	Mix_Chunk *sounds;


};

//void InitAudioSys();

#endif